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

	class VulkanScopeTimer {
	public:
		VulkanScopeTimer(VkCommandBuffer commands,VulkanProfiler* pf,const char* name);
		~VulkanScopeTimer();
	private:
		VulkanProfiler* profiler;
		VkCommandBuffer cmd;
		ScopeTimer timer;
	};

	class VulkanProfiler {
	public:
		

		void init(VkDevice _device,float timestampPeriod ,int perFramePoolSizes = 100);

		void grab_queries(VkCommandBuffer cmd);

		void cleanup();

		double get_stat(const std::string& name);
		VkQueryPool get_timer_pool();

		void add_timer(ScopeTimer& timer);

		uint32_t get_timestamp_id();

		std::unordered_map<std::string, double> stats;
	private:

		

		struct QueryFrameState {
			std::vector<ScopeTimer> frameTimers;
			VkQueryPool timerPool;
			uint32_t timerLast;
		};

		static constexpr int QUERY_FRAME_OVERLAP = 3;

		

		int currentFrame;
		float period;
		std::array<QueryFrameState,QUERY_FRAME_OVERLAP> queryFrames;

		VkDevice device;

		
	};
}

