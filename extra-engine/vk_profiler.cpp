#include <vk_profiler.h>
#include <algorithm>

namespace vkutil {


	void VulkanProfiler::init(VkDevice _device, float timestampPeriod, int perFramePoolSizes /*= 100*/)
	{
		period = timestampPeriod;
		device = _device;
		currentFrame = 0;
		int poolSize = perFramePoolSizes;

		VkQueryPoolCreateInfo queryPoolInfo = {};
		queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
		queryPoolInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
		queryPoolInfo.queryCount = poolSize;

		for (int i = 0; i < QUERY_FRAME_OVERLAP; i++)
		{
			vkCreateQueryPool(device, &queryPoolInfo, NULL, &queryFrames[i].timerPool);
			queryFrames[i].timerLast = 0;
		}
		queryPoolInfo.queryType = VK_QUERY_TYPE_PIPELINE_STATISTICS;
		for (int i = 0; i < QUERY_FRAME_OVERLAP; i++)
		{
			queryPoolInfo.pipelineStatistics = VK_QUERY_PIPELINE_STATISTIC_CLIPPING_INVOCATIONS_BIT;
			vkCreateQueryPool(device, &queryPoolInfo, NULL, &queryFrames[i].statPool);
			queryFrames[i].statLast = 0;
		}
	}


	void VulkanProfiler::grab_queries(VkCommandBuffer cmd)
	{
		int frame = currentFrame;
		currentFrame = (currentFrame + 1) % QUERY_FRAME_OVERLAP;

		vkCmdResetQueryPool(cmd, queryFrames[currentFrame].timerPool, 0, queryFrames[currentFrame].timerLast);
		queryFrames[currentFrame].timerLast = 0;
		queryFrames[currentFrame].frameTimers.clear();

		vkCmdResetQueryPool(cmd, queryFrames[currentFrame].statPool, 0, queryFrames[currentFrame].statLast);
		queryFrames[currentFrame].statLast = 0;
		queryFrames[currentFrame].statRecorders.clear();

		QueryFrameState& state = queryFrames[frame];
		std::vector<uint64_t> querystate;
		querystate.resize(state.timerLast);
		if (state.timerLast != 0)
		{
			// We use vkGetQueryResults to copy the results into a host visible buffer
			vkGetQueryPoolResults(
				device,
				state.timerPool,
				0,
				state.timerLast,
				querystate.size() * sizeof(uint64_t),
				querystate.data(),
				sizeof(uint64_t),
				// Store results a 64 bit values and wait until the results have been finished
				// If you don't want to wait, you can use VK_QUERY_RESULT_WITH_AVAILABILITY_BIT
				// which also returns the state of the result (ready) in the result
				VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
		}
		std::vector<uint64_t> statresults;
		statresults.resize(state.statLast);
		if (state.statLast != 0)
		{
			// We use vkGetQueryResults to copy the results into a host visible buffer
			vkGetQueryPoolResults(
				device,
				state.statPool,
				0,
				state.statLast,
				statresults.size() * sizeof(uint64_t),
				statresults.data(),
				sizeof(uint64_t),
				// Store results a 64 bit values and wait until the results have been finished
				// If you don't want to wait, you can use VK_QUERY_RESULT_WITH_AVAILABILITY_BIT
				// which also returns the state of the result (ready) in the result
				VK_QUERY_RESULT_64_BIT | VK_QUERY_RESULT_WAIT_BIT);
		}

		for (auto& timer : state.frameTimers) {
			uint64_t begin = querystate[timer.startTimestamp];
			uint64_t end = querystate[timer.endTimestamp];

			uint64_t timestamp = end - begin;
			//store timing queries as miliseconds
			timing[timer.name] = (double(timestamp) * period) / 1000000.0;
		}
		for (auto& st : state.statRecorders)
		{
			uint64_t result = statresults[st.query];

			stats[st.name] = static_cast<int32_t>(result);
		}
	}


	void VulkanProfiler::cleanup()
	{
		for (int i = 0; i < QUERY_FRAME_OVERLAP; i++)
		{
			vkDestroyQueryPool(device, queryFrames[i].timerPool, nullptr);
		}
	}

	double VulkanProfiler::get_stat(const std::string& name)
	{
		auto it = timing.find(name);
		if (it != timing.end())
		{
			return (*it).second;
		}
		else
		{
			return 0;
		}
	}


	VkQueryPool VulkanProfiler::get_timer_pool()
	{
		return queryFrames[currentFrame].timerPool;
	}


	VkQueryPool VulkanProfiler::get_stat_pool()
	{
		return queryFrames[currentFrame].statPool;
	}

	void VulkanProfiler::add_timer(ScopeTimer& timer)
	{
		queryFrames[currentFrame].frameTimers.push_back(timer);
	}


	void VulkanProfiler::add_stat(StatRecorder& timer)
	{
		queryFrames[currentFrame].statRecorders.push_back(timer);
	}

	uint32_t VulkanProfiler::get_timestamp_id()
	{
		uint32_t q = queryFrames[currentFrame].timerLast;
		queryFrames[currentFrame].timerLast++;
		return q;
	}


	uint32_t VulkanProfiler::get_stat_id()
	{
		uint32_t q = queryFrames[currentFrame].statLast;
		queryFrames[currentFrame].statLast++;
		return q;
	}

	VulkanScopeTimer::VulkanScopeTimer(VkCommandBuffer commands, VulkanProfiler* pf, const char* name)
	{
		cmd = commands;
		timer.name = name;
		profiler = pf;
		timer.startTimestamp = profiler->get_timestamp_id();

		VkQueryPool pool = profiler->get_timer_pool();

		vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, pool, timer.startTimestamp);
	}

	VulkanScopeTimer::~VulkanScopeTimer()
	{
		timer.endTimestamp = profiler->get_timestamp_id();
		VkQueryPool pool = profiler->get_timer_pool();
		vkCmdWriteTimestamp(cmd, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, pool, timer.endTimestamp);

		profiler->add_timer(timer);
	}

	VulkanPipelineStatRecorder::VulkanPipelineStatRecorder(VkCommandBuffer commands, VulkanProfiler* pf, const char* name)
	{
		cmd = commands;
		timer.name = name;
		
		profiler = pf;
		timer.query = profiler->get_stat_id();

		vkCmdBeginQuery(cmd, profiler->get_stat_pool(), timer.query,0);
	}


	VulkanPipelineStatRecorder::~VulkanPipelineStatRecorder()
	{
		VkQueryPool pool = profiler->get_stat_pool();
		vkCmdEndQuery(cmd, pool, timer.query);

		profiler->add_stat(timer);
	}

}