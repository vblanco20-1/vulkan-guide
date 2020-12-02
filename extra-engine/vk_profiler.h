// vulkan_guide.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <vk_types.h>
#include <vector>
#include <array>
#include <unordered_map>

namespace vkutil {
	class VulkanProfiler;

	struct ScopeTimer {
		uint32_t startTimestamp;
		uint32_t endTimestamp;
		std::string name;
	};
	
	struct StatRecorder {
		uint32_t query;
		
		std::string name;
	};

	class VulkanScopeTimer {
	public:
		VulkanScopeTimer(VkCommandBuffer commands,VulkanProfiler* pf,const char* name);
		~VulkanScopeTimer();
	private:
		VulkanProfiler* profiler;
		VkCommandBuffer cmd;
		ScopeTimer timer;
	};

	class VulkanPipelineStatRecorder {
	public:
		VulkanPipelineStatRecorder(VkCommandBuffer commands, VulkanProfiler* pf, const char* name);
		~VulkanPipelineStatRecorder();
	private:
		VulkanProfiler* profiler;
		VkCommandBuffer cmd;
		StatRecorder timer;
	};

	class VulkanProfiler {
	public:
		

		void init(VkDevice _device,float timestampPeriod ,int perFramePoolSizes = 100);

		void grab_queries(VkCommandBuffer cmd);

		void cleanup();

		double get_stat(const std::string& name);
		VkQueryPool get_timer_pool();
		VkQueryPool get_stat_pool();

		void add_timer(ScopeTimer& timer);

		void add_stat(StatRecorder& timer);
		uint32_t get_timestamp_id();
		uint32_t get_stat_id();

		std::unordered_map<std::string, double> timing;
		std::unordered_map<std::string, int32_t> stats;
	private:

		

		struct QueryFrameState {
			std::vector<ScopeTimer> frameTimers;
			VkQueryPool timerPool;
			uint32_t timerLast;

			std::vector<StatRecorder> statRecorders;
			VkQueryPool statPool;
			uint32_t statLast;
		};

		static constexpr int QUERY_FRAME_OVERLAP = 3;

		

		int currentFrame;
		float period;
		std::array<QueryFrameState,QUERY_FRAME_OVERLAP> queryFrames;

		VkDevice device;

		
	};
}

