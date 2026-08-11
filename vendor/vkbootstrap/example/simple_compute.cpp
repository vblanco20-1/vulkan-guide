#include <VkBootstrap.h>
#include <iostream>
#include <fstream>
#include "example_config.h"

struct Init {
    vkb::Instance instance;
    vkb::InstanceDispatchTable inst_disp;
    vkb::Device device;
    vkb::DispatchTable disp;
};

struct Data {
    VkQueue queue;

    VkPipelineLayout pipeline_layout;
    VkPipeline compute_pipeline;

    VkCommandPool command_pool;
    VkCommandBuffer command_buffer;

    VkBuffer buffer_a;
    VkDeviceMemory memory_a;
    VkBuffer buffer_b;
    VkDeviceMemory memory_b;
    uint32_t* buffer_ptr_a;
    uint32_t* buffer_ptr_b;

    VkDescriptorSet descriptor_set;
    VkDescriptorPool descriptor_pool;
    VkDescriptorSetLayout descriptor_set_layout;
};

int device_initialization(Init& init) {
    vkb::InstanceBuilder instance_builder;
    auto instance_ret = instance_builder.use_default_debug_messenger()
                            .request_validation_layers()
                            .set_headless() // Skip vk-bootstrap trying to create WSI for you
                            .build();
    if (!instance_ret) {
        std::cout << instance_ret.error().message() << "\n";
        return -1;
    }
    init.instance = instance_ret.value();

    init.inst_disp = init.instance.make_table();

    vkb::PhysicalDeviceSelector phys_device_selector(init.instance);
    auto phys_device_ret = phys_device_selector.select();
    if (!phys_device_ret) {
        std::cout << phys_device_ret.error().message() << "\n";
        if (phys_device_ret.error() == vkb::PhysicalDeviceError::no_suitable_device) {
            const auto& detailed_reasons = phys_device_ret.detailed_failure_reasons();
            if (!detailed_reasons.empty()) {
                std::cerr << "GPU Selection failure reasons:\n";
                for (const std::string& reason : detailed_reasons) {
                    std::cerr << reason << "\n";
                }
            }
        }
        return -1;
    }
    vkb::PhysicalDevice physical_device = phys_device_ret.value();

    vkb::DeviceBuilder device_builder{ physical_device };
    auto device_ret = device_builder.build();
    if (!device_ret) {
        std::cout << device_ret.error().message() << "\n";
        return -1;
    }
    init.device = device_ret.value();

    init.disp = init.device.make_table();

    return 0;
}

int get_queues(Init& init, Data& data) {
    // Compute can be done on a Grahpics queue
    // See require_dedicated_compute_queue() if want a dedicated compute queue
    auto gq = init.device.get_queue(vkb::QueueType::graphics);
    if (!gq.has_value()) {
        std::cout << "failed to get queue: " << gq.error().message() << "\n";
        return -1;
    }
    data.queue = gq.value();
    return 0;
}

std::vector<char> readFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error(std::string("Failed to open file ") + filename + "!");
    }

    size_t file_size = (size_t)file.tellg();
    std::vector<char> buffer(file_size);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(file_size));
    file.close();

    return buffer;
}

void create_descriptor(Init& init, Data& data) {
    VkDescriptorPoolSize pool_size = { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2 };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = 0;
    pool_info.maxSets = 1;
    pool_info.poolSizeCount = 1;
    pool_info.pPoolSizes = &pool_size;
    init.disp.createDescriptorPool(&pool_info, nullptr, &data.descriptor_pool);

    VkDescriptorSetLayoutBinding binding = { 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2, VK_SHADER_STAGE_ALL, nullptr };
    VkDescriptorSetLayoutCreateInfo dsl_info = {};
    dsl_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dsl_info.flags = 0;
    dsl_info.bindingCount = 1;
    dsl_info.pBindings = &binding;
    init.disp.createDescriptorSetLayout(&dsl_info, nullptr, &data.descriptor_set_layout);

    VkDescriptorSetAllocateInfo ds_allocate_info = {};
    ds_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    ds_allocate_info.descriptorPool = data.descriptor_pool;
    ds_allocate_info.descriptorSetCount = 1;
    ds_allocate_info.pSetLayouts = &data.descriptor_set_layout;
    init.disp.allocateDescriptorSets(&ds_allocate_info, &data.descriptor_set);
}

void create_compute_pipeline(Init& init, Data& data) {
    auto spv_code = readFile(std::string(EXAMPLE_SOURCE_DIRECTORY) + "/example/shaders/simple_compute.comp.spv");

    VkShaderModuleCreateInfo create_info = {};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.codeSize = spv_code.size();
    create_info.pCode = reinterpret_cast<const uint32_t*>(spv_code.data());

    VkShaderModule shader_module;
    init.disp.createShaderModule(&create_info, nullptr, &shader_module);

    VkPipelineShaderStageCreateInfo shader_stage_info = {};
    shader_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shader_stage_info.module = shader_module;
    shader_stage_info.pName = "main";

    VkPipelineLayoutCreateInfo pipeline_layout_info = {};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &data.descriptor_set_layout;
    pipeline_layout_info.pushConstantRangeCount = 0;
    init.disp.createPipelineLayout(&pipeline_layout_info, nullptr, &data.pipeline_layout);

    VkComputePipelineCreateInfo pipeline_info = {};
    pipeline_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipeline_info.stage = shader_stage_info;
    pipeline_info.layout = data.pipeline_layout;
    pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
    init.disp.createComputePipelines(VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &data.compute_pipeline);

    init.disp.destroyShaderModule(shader_module, nullptr);
}

void create_command_pool(Init& init, Data& data) {
    VkCommandPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = init.device.get_queue_index(vkb::QueueType::graphics).value();
    init.disp.createCommandPool(&pool_info, nullptr, &data.command_pool);

    VkCommandBufferAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocate_info.commandPool = data.command_pool;
    allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocate_info.commandBufferCount = 1;
    init.disp.allocateCommandBuffers(&allocate_info, &data.command_buffer);
}

uint32_t get_memory_index(Init& init, const uint32_t type_bits) {
    uint32_t type_mask = type_bits;
    VkPhysicalDeviceMemoryProperties mem_props = init.device.physical_device.memory_properties;
    for (uint32_t i = 0; i < mem_props.memoryTypeCount; i++) {
        if ((type_mask & 1) == 1) {
            if ((mem_props.memoryTypes[i].propertyFlags &
                    (VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) != 0) {
                return i;
            }
        }
        type_mask >>= 1;
    }
    return UINT32_MAX;
}

void create_buffers(Init& init, Data& data) {
    VkBufferCreateInfo buffer_info = {};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.size = sizeof(uint32_t);
    buffer_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    init.disp.createBuffer(&buffer_info, nullptr, &data.buffer_a);

    VkMemoryRequirements reqs;
    init.disp.getBufferMemoryRequirements(data.buffer_a, &reqs);

    VkMemoryAllocateInfo allocate_info = {};
    allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocate_info.allocationSize = reqs.size;
    allocate_info.memoryTypeIndex = get_memory_index(init, reqs.memoryTypeBits);
    init.disp.allocateMemory(&allocate_info, nullptr, &data.memory_a);

    init.disp.bindBufferMemory(data.buffer_a, data.memory_a, 0);

    // Same for 2nd buffer
    init.disp.createBuffer(&buffer_info, nullptr, &data.buffer_b);
    init.disp.getBufferMemoryRequirements(data.buffer_b, &reqs);
    allocate_info.allocationSize = reqs.size;
    allocate_info.memoryTypeIndex = get_memory_index(init, reqs.memoryTypeBits);
    init.disp.allocateMemory(&allocate_info, nullptr, &data.memory_b);
    init.disp.bindBufferMemory(data.buffer_b, data.memory_b, 0);

    // Update buffers with data
    // (We are doing the world's most inefficient adding of 42 + 58 to get 100)
    init.disp.mapMemory(data.memory_a, 0, VK_WHOLE_SIZE, 0, ((void**)&data.buffer_ptr_a));
    init.disp.mapMemory(data.memory_b, 0, VK_WHOLE_SIZE, 0, ((void**)&data.buffer_ptr_b));
    *data.buffer_ptr_a = 42;
    *data.buffer_ptr_b = 58;

    VkDescriptorBufferInfo buffer_infos[2] = {
        { data.buffer_a, 0, VK_WHOLE_SIZE },
        { data.buffer_b, 0, VK_WHOLE_SIZE },
    };
    VkWriteDescriptorSet write_ds = {};
    write_ds.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write_ds.dstSet = data.descriptor_set;
    write_ds.dstBinding = 0;
    write_ds.dstArrayElement = 0;
    write_ds.descriptorCount = 2;
    write_ds.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    write_ds.pBufferInfo = buffer_infos;
    init.disp.updateDescriptorSets(1, &write_ds, 0, nullptr);
}

void create_and_submit_work(Init& init, Data& data) {
    VkCommandBufferBeginInfo begin_info = {};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    init.disp.beginCommandBuffer(data.command_buffer, &begin_info);
    init.disp.cmdBindPipeline(data.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, data.compute_pipeline);
    init.disp.cmdBindDescriptorSets(
        data.command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, data.pipeline_layout, 0, 1, &data.descriptor_set, 0, nullptr);
    init.disp.cmdDispatch(data.command_buffer, 1, 1, 1);
    init.disp.endCommandBuffer(data.command_buffer);

    VkSubmitInfo submit_info = {};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &data.command_buffer;
    init.disp.queueSubmit(data.queue, 1, &submit_info, VK_NULL_HANDLE);

    init.disp.deviceWaitIdle();

    // Lets see if we did it
    std::cout << "Expected a + b to be 100\nResult == " << *data.buffer_ptr_a << "\n";
}

void cleanup(Init& init, Data& data) {
    init.disp.destroyCommandPool(data.command_pool, nullptr);
    init.disp.destroyPipeline(data.compute_pipeline, nullptr);
    init.disp.destroyPipelineLayout(data.pipeline_layout, nullptr);
    init.disp.destroyBuffer(data.buffer_a, nullptr);
    init.disp.destroyBuffer(data.buffer_b, nullptr);
    init.disp.freeMemory(data.memory_a, nullptr);
    init.disp.freeMemory(data.memory_b, nullptr);
    init.disp.destroyDescriptorPool(data.descriptor_pool, nullptr);
    init.disp.destroyDescriptorSetLayout(data.descriptor_set_layout, nullptr);

    vkb::destroy_device(init.device);
    vkb::destroy_instance(init.instance);
}

int main() {
    Init init;
    Data data;

    if (0 != device_initialization(init)) return -1;
    if (0 != get_queues(init, data)) return -1;
    // We are not error checking here, not good practice, but this is just an example program!
    create_descriptor(init, data);
    create_compute_pipeline(init, data);
    create_command_pool(init, data);
    create_buffers(init, data);
    create_and_submit_work(init, data);
    cleanup(init, data);
    return 0;
}
