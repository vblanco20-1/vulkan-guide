#include <vk_engine.h>

#include <SDL.h>
#include <SDL_vulkan.h>

int main(int argc, char* argv[])
{
	VulkanEngine engine;

	engine.init();	
	
	engine.run();	

	engine.cleanup();	

	return 0;
}
