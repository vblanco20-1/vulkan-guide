---
layout: default
title: Multithreading for game engines WIP
parent: Extra Chapter
nav_order: 40
---

## Multithreading Overview

For the last 20 years, computers and game consoles have had multiple cores in their CPUs. Over time, the number of cores has increased, with the new consoles having 8 cores with hyperthreading, and PCs getting more and more cores, with things like some ARM servers hitting 80 real cores in a single CPU. Meanwhile, the single-thread performance of CPUs has been relatively stagnant for quite a while, hitting a GHZ barrier of 4-5 gz of clock speed in the CPUs. This means that applications these days need to be programmed for multiple cores to really use the full power of the CPU.

A multicore CPU generally has multiple "standalone" cores, each of them being a full CPU on its own. Each of the cores can execute an arbitrary program on its own. If the CPU has hyperthreading/SMT, the CPU will not execute one thread of program instructions, but multiple (often 2). When this happens the inner resources of the CPU (memory units, math units, logic units, caches) will be shared between those multiple threads.

The different cores on the CPU work on their own, and the CPU synchronizes the memory writes from one core so that other cores can also see it if it detects that one core is writing to the same memory location a different core is also writing into or reading from. That syncronization has a cost, so whenever you program multithreaded programs, its important to try to avoid having multiple threads writing to the same memory, or one thread writing and other reading.

For correct syncronization of operations, CPUs also have specific instructions that can synchronize multiple cores, like atomic instructions. Those instructions are the backbone of the syncronization primitives that are used to communicate between threads.

If you want to learn more about the hardware details of CPUs and how they map to Cpp programming, this CPPCon presentation explains it nicely [1](https://www.youtube.com/watch?v=BP6NxVxDQIs);


## Ways of using multithreading in game engines.
At first, game engines were completely singlethreaded, and wouldnt use multiple cores at all. Over time, the engines were programmed to use more and more cores, with patterns and architectures that map to that amount of cores.

The first and most classic way of multithreading a game engine is to make multiple threads, and have each of them perform their own task.

For example, you have a Game Thread, which runs all of the gameplay logic and AI. Then you have a Render Thread that handles all the code that deals with rendering, preparing objects to draw and executing graphics commands. 

An example we can see of this is Unreal Engine 2, 3, and 4. Unreal Engine 4 is open source, so it can be a good example. Doom 3 (2004) engine as explained on this article shows it clearly too [2](https://fabiensanglard.net/doom3_bfg/threading.php).

Unreal Engine 4 will have a Game Thread and a Render Thread as main, and then a few others for things such as helpers, audio, or loading. The Game Thread in Unreal Engine will run all of the gameplay logic that developers write in Blueprints and Cpp, and at the end of each frame, it will synchronize the positions and state of the objects in the world with the Render Thread, which will do all of the rendering logic and make sure to display them.

While this approach is very popular and very easy to use, it has the drawback of scaling terribly. You will commonly see that Unreal Engine games struggle scaling past 4 cores, and in consoles the performance is much lower that it could be due to not filling the 8 cores with work. Another issue with this model is that if you have one of the threads have more work than the others, then the entire simulation will wait. In unreal engine 4, the Game THread and Render Thread are synced at each frame, so if either of them is slow, both will be slowed as the run at the same time. A game that has lots of blueprint usage and AI calculations in UE4 will have the Game Thread busy doing work in 1 core, and then every other core in the machine unused.

A common approach of enhancing this architecture is to move it more into a fork/join approach, where you have a main execution thread, and at some points, parts of the work is split between threads. Unreal Engine does this for animation and physics. While the Game Thread is the one in charge of the whole game logic part of the engine, when it reaches the point where it has to do animations, it will split the animations to calculate into small tasks, and distribute those across helper threads in other cores. This way while it still has a main timeline of execution, there are points where it gets extra help from the unused cores. This improves scalability, but its still not good enough as the rest of the frame is still singlethreaded.


Mostly since the ps4 generation of consoles, which have 8 very weak cores, architectures have evolved into trying to make sure all the cores are working on something and doing something useful. A lot of game engines have moved to a Task based system for that purpose. In there, you dont dedicate 1 thread to do one thing, but instead split your work into small sections, and then have multiple threads work on those sections on their own, merging the results after the tasks finish. Unlike the fork-join approach of having one dedicated thread and having it ship off work to helpers, you do everything on the helpers for the most part. Your main timeline of operations is created as a graph of tasks to do, and then those are distributed across cores. A task cant start until all of its predecessor tasks are finished. If a task system is used well, it grants really good scalability as everything automatically distributes to however many cores are available. A great example of this is Doom Eternal, where you can see it smoothly scaling from PCs with 4 cores to PCs with 16 cores. Some great talks from GDC about it are Naughty Dog "Parallelizing the Naughty Dog Engine Using Fibers" [3](https://www.gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine) and the 2 Destiny Engine talks [4](https://www.youtube.com/watch?v=v2Q_zHG3vqg)  [5](https://www.youtube.com/watch?v=0nTDFLMLX9k)


## In practice.
Cpp since version 11 has a lot of utilities in the standard library that we can use for multithreading. The starter one is `std::thread`. This will wrap around a thread from the operating system. Threads are expensive to create and creating more threads than you have cores will decrease performance due to overhead. Even then, using a `std::thread` to create something like explained above with dedicated threads can be straightforward. Detailed info here [6](https://en.cppreference.com/w/cpp/thread/thread)
Pseudocode
```cpp

void GameLoop()
{
    while(!finish)
    {
        SyncRenderThread();

        PerformGameLogic();

        SyncRenderThread();
    }
}

void main(){


    std::thread gamethread(GameLoop);

    while(!finish)
    {
        SyncGameThread();

        PerformRenderLogic();

        SyncGameThread();

        CopyGameThreadDataToRenderer();
    }
}
```

In here, the mainloop renders, and each frame is synced with the gamethread. Once both threads finish their work, the renderthread copies the gamethread data into its internal structures.
In a design like this, the game thread CANT access the renderer internal structures, and the renderer cant access the data of the gamethread until the gamethread has finished executing. The 2 threads are working on their own, and only sharing at the end of the frame.

With the renderer design in vkguide gpudriven chapter, something like this can be implemented in a straightforward way. The renderer doesnt care about game logic, so you can have your game logic however you want, and at the sync point of each frame you create/delete/move the renderables of the render engine.

This design will only scale to 2 cores, so we need to find a way to split things more. Lets take a deeper look into `PerformGameLogic()`, and see if there are things we can do to make it scale more.


Pseudocode
```cpp
void PerformGameLogic(){

    input->QueryPlayerInput();
    player->UpdatePlayerCharacter();

    for(AICharacter* AiChar : AICharacters)
    {
         AiChar->UpdateAI();
    }

    for(WorldObject* Obj : Objects)
    {
        Obj->Update();
    }


    for(Particle* Part : ParticleSystems)
    {
        Part->UpdateParticles();
    }

    for(AICharacter* AiChar : AICharacters)
    {
         AiChar->UpdateAnimation();
    }

    physicsSystem->UpdatePhysics();
}
```

In our main game loop, we have a sequence of things that we require to do for each gameplay frame. We need to query input, update AIs, animate objects... . All of this is a lot of work, so we need to find something that is a "standalone" task that we can safely ship to other threads. After looking into the codebase, we find that UpdateAnimation() on each AiCharacter is something that only accesses that character, and as such is safe if we ship it to multiple threads.

There is something we can use here that will work great, known as a ParallelFor. ParallelFor is a very common multithreading primitive that nearly every multithreading library implements. A ParallelFor will split each of the iterations of a for loop into multiple cores automatically. Given that UpdateAnimation is done on each character and its standalone, we can use it here. 

Cpp17 added parallelized std::algorithms, which are awesome, but only implemented in Visual Studio for now. You can play around with them if you use VS, but if you want multiplatform, you will need to find alternatives. You can find some more information about the parallel algorithms here [7](https://devblogs.microsoft.com/cppblog/using-c17-parallel-algorithms-for-better-performance/), and if you want to learn about std::algorithms in general, this cppcon talk gives a overview [8](https://www.youtube.com/watch?v=2olsGf6JIkU)

One of the parallel algorithms is std::for_each(), which is the parallel for we want. If we use that, then the code can now look like this.

```cpp

std::for_each(std::execution::par, //we need the execution::par for this foreach to execute parallel. Its on the <execution> header
            AICharacters.begin(),
            AICharacters.end(),
            [](AICharacter* AiChar){ //cpp lambda that executes for each element in the AICharacters container.
                AiChar->UpdateAnimation();
            });
```

With this, our code is now similar to unreal engine 4, in that it has a game thread that is on its own for a while, but a section of it gets parallelized across cores. You would also do exactly the same thing on the renderer for things that can be parallelized.

But we still want more parallelism, as with this model only a small amount of the frame uses other cores, so we can try to see if we can convert it into a task system.

As stand-in for a task system, we will use `std::async`. `std::async` creates a lightweight thread, so we can create many of them without too much issue, as they are much cheaper than creating a `std::thread`. For medium/small lived tasks, they can work nicely. Make sure to check how they run in your platform, as they are very well implemented in Visual Studio, but in GCC/clang their implementation might not be as good. As with the parallel algorithms, you can find a lot of libraries that implement something very similar.

For a library that works quite well, you can try Taskflow, which has a `std::async` but better equivalent, alongside many more features [9](https://github.com/taskflow/taskflow)


pseudocode
```cpp

auto player_tasks = std::async(std::launch::async,
    [](){
        input->QueryPlayerInput();
        player->UpdatePlayerCharacter();
    });

auto ai_task = std::async(std::launch::async,
    [](){
    //we cant parallelize all the AIs together as they communicate with each other, so no parallel for here.
    for(AICharacter* AiChar : AICharacters)
    {
         AiChar->UpdateAI();
    }
    });

//lets wait until both player and AI asyncs are finished before continuing
player_tasks.wait();
ai_task.wait();
   
//world objects cant be updated in parallel as they affect each other too
for(WorldObject* Obj : Objects)
{
    Obj->Update();
}

// particles is standalone AND we can update each particle on its own, so we can combine async with parallel for
auto particles_task = std::async(std::launch::async,
    [](){
        std::for_each(std::execution::par, 
            ParticleSystems.begin(),
            ParticleSystems.end(),
            [](Particle* Part){ 
                 Part->UpdateParticles();
            });   
    });

// same with animation
auto animation_task = std::async(std::launch::async,
    [](){
        std::for_each(std::execution::par, 
            AICharacters.begin(),
            AICharacters.end(),
            [](AICharacter* AiChar){ 
                  AiChar->UpdateAnimation();
            });   
    });

//physics can also be updated on its own
auto physics_task = std::async(std::launch::async,
    [](){
       physicsSystem->UpdatePhysics();
    });
    
//synchronize the 3 tasks
particles_task.wait();
animation_task.wait();
physics_task.wait();

```

With this, we are defining a graph of tasks, and their dependencies for execution. We manage to run 3 tasks in parallel at the end, even with 2 of them doing parallel fors, so the threading in here is far superior to the model before. While we still have a very sad fully singlethreaded section in the ObjectUpdate section, at least the other things are reasonably parallelized. 

Having sections of the frame that bottleneck the tasks and have to run in one core only is very common, so what most engines do is that they still have game thread and render thread, but each of them has its own parallel tasks. Hopefully there are enough tasks that can run on their own to fill all of the cores.

While the example here uses async, you really want to use better libraries for this. Its also probable that you will want a better control over execution instead of launching many asyncs and parallel for. Using taskflow here would work great.

## Identifying tasks.
While we have been commenting that things like UpdatePhysics() can run overlapped, things are never so simple in practice. 
Identifying what tasks can run at the same time as others is something very hard to do in a project, and in Cpp, its not possible to validate it. If you use rust, this comes automatically thanks to the borrowcheck model. 

A common way to know that a given task can run alongside another one is to "package" the data the task could need, and make sure that the task NEVER accesses any other data outside of it. In the example with game thread and render thread, we have a Renderer class where all the render data is stored, and the game thread never touches that. We also only let the render thread access the gamethread data at a very specific point in the frame where we know the gamethread isnt working on it.

For most tasks, there are places where we just cant guarantee that only one task is working on it at a time, and so we need a way of syncronizing data beetwen the different tasks run at the same time on multiple cores.

## Syncronizing data.
There are a lot of ways of syncronizing the data tasks work on. Generally you really want to minimize the shared data a lot, as its a source of bugs, and can be horrible if not synchronized properly. There are a few patterns that do work quite well that can be used. 

A common one is to have tasks publish messages into queues, and then another task can read it at other point. Lets say that our Particles from above need to be deleted, but the particles are stored in an array, and deleting a particle from that array when the other threads are working on other particles of the same array is a guaranteed way to make the program crash. We could insert the particle IDs into a shared synchronized queue, and after all particles finished their work, delete the particles from that queue.

We are not going to look at the implementation details of the queue. Lets say its a magic queue where you can safely push elements into from multiple cores at once. There are lots of those queues around. Ive used Moodycamel Concurrent Queue a lot for this purpose [10](https://github.com/cameron314/concurrentqueue)

```cpp
parallel_queue<Particle*> deletion_queue;

// particles is standalone AND we can update each particle on its own, so we can combine async with parallel for
auto particles_task = std::async(std::launch::async,
    [](){
        std::for_each(std::execution::par, 
            ParticleSystems.begin(),
            ParticleSystems.end(),
            [](Particle* Part){ 
                 Part->UpdateParticles();

                if(Part->IsDead)
                {
                    deletion_queue.push(Part);
                }
            });   
    });

//the other tasks.


particles_task.wait();

//after waiting for the particles task, there is nothing else that touches the particles, so its safe to apply the deletions
for(Particle* Part : deletion_queue)
{
    Part->Remove();
}


```

This is a very common pattern in multithreaded code, and very useful, but like everything, it has its drawbacks. Having queues like this will increase the memory usage of the application due to all the data duplication, and inserting the data into these threadsafe queues is not free.

## Atomics

The core syncronization primitives to communicate data between cores are atomic operations, and Cpp past 11 has them integrated into the STL. 
Atomic operations are a specific set of instructions that are guaranteed to work well(as specified) even if multiple cores are doing things at once. Things like parallel queues and mutexes are implemented with them. 
Atomic operations are often significantly more expensive than normal operations, so you cant just make every variable in your application an atomic one, as that would harm performance a lot. They are most often used to aggregate the data from multiple threads or do some light syncronization. 

As an example of what atomics are, we are going to continue with the example above of the particle system, and we will use atomic-add to add how many vertices we have across all particles, without splitting the parallel for.

For that use, we are going to be using `std::atomic<int>`. You can have atomic variables of multiple base types like integers and floats.

```cpp

//create variable to hold how many billboards we have
std::atomic<int> vertex{0};

std::for_each(std::execution::par, 
            ParticleSystems.begin(),
            ParticleSystems.end(),
            [](Particle* Part){ 
                Part->UpdateParticles();

                if(Part->IsDead)
                {
                    deletion_queue.push(Part);
                }
                else{
                //add the number of vertices in this system
                vertexAmount += Part->vertexCount;
                }
            });   

```

In this example, if we were using a normal integer, the count will very likely be wrong, as each thread will add a different value and its very likely that one thread will override another thread, but in here, we are using `atomic<int>`, which is guaranteed to have the correct value in a case like this. Atomic numbers also implement more abstract operations such as Compare and Swap, and Fetch Add, which can be used to implement synchronized data structures, but doing that properly is for the experts, as its some of the hardest things you can try to implement yourself.

When used wrong, atomic variables will error in very subtle ways depending on the hardware of the CPU. Debugging this sort of errors can be hard to do. 

A great presentation about using atomics to implement synchronized data structures, and how hard it really is, is this talk from Cppcon. [11](https://www.youtube.com/watch?v=c1gO9aB9nbs). For a more in-depth explanation about how exactly std atomic works, this other talk explains it well. [12](https://www.youtube.com/watch?v=ZQFzMfHIxng)

To actually synchronize data structures or multithreaded access to something its better to use mutexes.


## Mutex

A mutex is a higher level primitive that is used to control the execution flow on threads. You can use them to make sure that a given operation is only being executed by one thread at a time. API details can be found here [13](https://en.cppreference.com/w/cpp/thread/mutex)

Mutexes are implemented using atomics, but if they block a thread for a long time, they can ask the OS to put that thread on the background and let a different thread execute. This can be expensive, so mutexes are best used on operations that you know wont block too much.

Continuing with the example, we are going to implement the parallel queue above as a normal vector, but protected with a mutex.


```cpp

//create variable to hold how many billboards we have
std::vector<Particle*> deletion_list;

//declare a mutex for the syncronization.
std::mutex deletion_mutex;

std::for_each(std::execution::par, 
            ParticleSystems.begin(),
            ParticleSystems.end(),
            [](Particle* Part){ 
                Part->UpdateParticles();

                if(Part->IsDead)
                {
                    //lock the mutex. If the mutex is already locked, the current thread will wait until it unlocks
                    deletion_mutex.lock();

                    //only one thread at a time will execute this line
                    deletion_list.push(Part);

                    //dont forget to unlock the mutex!!!!!
                    deletion_mutex.unlock();

                }                
            });   
```

Cpp mutexes have a Lock and Unlock function, and there is also a try_lock function that returns false if the mutex is locked and cant be locked again.

Only one thread at a time can lock the mutex. If a second thread tries to lock the mutex, then that second thread will have to wait until the mutex unlocks. This can be used to define sections of code that are guaranteed to only be executed for one thread at a time.

Whenever mutexes are used, its very important that they are locked for a short amount of time, and unlocked as soon as possible. If you are using a task system, its imperative that all mutexes are unlocked before the task finishes, as if a task finishes without unlocking a mutex it will block everything.

Mutexes have the great issue that if they are used wrong, the program can completely block itself. This is known as a Deadlock, and its very easy to have it on code that looks fine but at some point in some case 2 threads can lock each other.

There are ways to avoid deadlocks, one of the most straightforward one is that any time you use a mutex you shouldnt take another mutex unless you know what you are doing, and any time you lock a mutex you unlock it asap.

As calling lock/unlock manually can be done wrong very easily, specially in cases where the function returns or there is an exception, Cpp STL has `std::lock_guard`, which does it automatically. Using it, the code above would look like this

```cpp
if(Part->IsDead)
{
    //the mutex is locked in the constructor of the lock_guard
    std::lock_guard<std::mutex> lock(deletion_mutex);

    //only one thread at a time will execute this line
    deletion_list.push(Part);

    //automatically unlocks 
}          
```


## Multithreading Vulkan
We have explained the ways one can parallelize things in the engine, but what about GPU calls themselves? If we were talking about OpenGL, there would be absolutely nothing you can do. In OpenGL or other older APIs, its only possible to do API calls from one thread. Not even one thread at a time, but one specific thread. For those apis, renderers often created a dedicated OpenGL/API thread that would execute the commands that other threads sent to it. You can see that on both the Doom3 engine and the UE4 engine.

On more modern APIs like Vulkan and DX12, we have a design that is meant to be used from multiple cores. In the case of vulkan, the spec defines some rules about what resources must be protected and not used at once. We are going to see some typical examples of the kind of things you can multithread in vulkan, and their rules.

For compiling pipelines, vkCreateShaderModule and vkCreateGraphicsPipeline are both allowed to be called from multiple threads at once. A common approach for multithreaded shader compilation is to have a background thread dedicated to it, with it constantly looking into a parallel queue to receive compilation requests, and putting the compiled pipelines into another queue that then the main renderthread will connect to the simulation. This is very important to do if you want to have an engine that doesnt have a lot of hitching. Compiling shader pipelines can take a very long time, so if you have to compile pipelines at runtime outside of a load screen, then you need to implement such a multithreaded async compile scheme for your game to work well.

For descriptor set building, that can also be done from multiple threads, as long as the access to the DescriptorPool you are using to allocate the descriptor sets is syncronized and not used from multiple threads at once. A very common approach for it is to keep multiple DescriptorPools around, and whenever a thread needs to allocate descriptors, it will "grab" one of the multiple availible descriptor pools, use it, and then return it so that other threads can use the same pool.

Command submission and recording is also completely parallel, but there are some rules around it. A Thread can only do VkQueueSubmit to a given queue at once. If you want multiple threads doing VkQueueSubmit, then you need to create multiple queues. As the number of queues can be as low as 1 in some devices, what engines tend to do for this is to do something similar to the pipeline compile thread or the OpenGL api call thread, and have a thread dedicated to just doing VkQueueSubmit. As VkQueueSubmit is a very expensive operation, this can bring a very nice speedup as the time spent executing that call is done in a second thread and the main logic of the engine doesnt have to stop.

When you record command buffers, a thread can only record commands created from one command pool. While you can create multiple command buffers from a command pool, you cant fill those commands from multiple threads. If you want to record command buffers from multiple threads, then you will need more command pools, one per thread. 

Vulkan command buffers have a system for primary and secondary commands. The primary commands are the ones that open and close RenderPasses, and get submitted to a queue. Secondary commands can be used to create "child" command buffers that execute as part of a main one. Their main purpose is multithreading.

Lets say you have a ForwardPass renderpass. Before making the main command buffer that will get submitted, you make sure to get 3 command pools, allocate 3 command buffers from them, and then send them to 3 worker threads to record one third of the forward pass commands each. Once the 3 workers have finished their work, you have 3 secondary command buffers, each of them recording a third of the ForwardPass renderpass. Then you can finish recording the main command buffer which will execute those 3 subpasses on its renderpass.

pseudocode
```cpp

VkCommandBuffer primaryBuffer = allocate_buffer( main_command_pool );

vkCmdBegin(primaryBuffer, ... );

VkRenderPassBeginInfo renderPassBeginInfo = init_renderpass(forward_pass);


//begin render pass from the main execution
vkCmdBeginRenderPass(primaryBuffer, forward_pass);


//when allocating secondary commands, we need to fill inheritance info struct, to tell the commands what renderpass is their parent.
VkCommandBufferInheritanceInfo inheritanceInfo = init_inheritance_info(forward_pass);


//we can now record the secondary commands
std::array<VkCommandBuffer, 3> subcommands;

//create 3 parallel tasks to each render a section
parallel_for(3,[](int i)
{
    //secondary commands have to be created with the inheritance info that links to renderpass
    subcommands[i] = allocate_buffer( worker_command_pools[i],inheritanceInfo );

    build_scene_section(i, subcommands[i]);
});
 
//now that the workers have finished writing the commands, we can add their contents to the main command buffer
vkCmdExecuteCommands(primaryBuffer, subcommands.size(), subcommands.data());

//finish the buffer
vkCmdEndRenderPass(primaryBuffer);
vkEndCommandBuffer(primaryBuffer);
```

This scheme of synchronizing the Vulkan subcommands and their resources can be tricky to get right, and Vulkan command encoding is very very fast, so you arent optimizing much here. Some engines implement their own command buffer abstraction that is easier to handle from multiple threads, and then a recording thread will very quickly transform those abstracted commands into vulkan. 

Because vulkan commands record so fast, recording from multiple threads wont be a big optimization. But your renderer isnt just recording commands in a deep loop, you have to do a lot more work. By splitting the command recording across multiple threads, then you can multithread your renderer internals in general much better. Doom eternal is known to do this.

Data upload is another section that is very often multithreaded. In here, you have a dedicated IO thread that will load assets to disk, and said IO thread will have its own queue and command allocators, hopefully a transfer queue. This way it is possible to upload assets at a speed completely separated from the main frame loop, so if it takes half a second to upload a set of big textures, you dont have a hitch. 
To do that, you need to create a transfer or async-compute queue (if available), and dedicate that one to the loader thread. Once you have that, its similar to what was commented on the pipeline compiler thread, and you have an IO thread that communicates through a parallel queue with the main simulation loop to upload data in an asynchronous way. Once a transfer has been uploaded, and checked that it has finished with a Fence, then the IO thread can send the info to the main loop, and then the engine can connect the new textures or models into the renderer.





## Links
* [1]  [CppCon 2016, “Want fast C++? Know your hardware!"](https://www.youtube.com/watch?v=BP6NxVxDQIs)
* [2]  [Fabien Sanglard Doom 3 engine overview](https://fabiensanglard.net/doom3_bfg/threading.php)
* [3]  [GDC Parallelizing the Naughty Dog Engine Using Fibers](https://www.gdcvault.com/play/1022186/Parallelizing-the-Naughty-Dog-Engine)
* [4]  [GDC Multithreading the Entire Destiny Engine](https://www.youtube.com/watch?v=v2Q_zHG3vqg)
* [5]  [GDC Destiny's Multithreaded Rendering Architecture](https://www.youtube.com/watch?v=0nTDFLMLX9k)
* [6]  [Cpp reference std::thread](https://en.cppreference.com/w/cpp/thread/thread)  
* [7]  [MSVC blog, Using C++17 Parallel Algorithms for Better Performance](https://devblogs.microsoft.com/cppblog/using-c17-parallel-algorithms-for-better-performance/)
* [8]  [CppCon 2018, “105 STL Algorithms in Less Than an Hour”](https://www.youtube.com/watch?v=2olsGf6JIkU)
* [9]  [Github taskflow](https://github.com/taskflow/taskflow)
* [10] [Github parallel queue](https://github.com/cameron314/concurrentqueue)
* [11] [CppCon 2014, "Lock-Free Programming (or, Juggling Razor Blades), Part I"](https://www.youtube.com/watch?v=c1gO9aB9nbs)
* [12] [CppCon 2017, “C++ atomics, from basic to advanced. What do they really do?”](https://www.youtube.com/watch?v=ZQFzMfHIxng)
* [13] [Cpp reference std::mutex](https://en.cppreference.com/w/cpp/thread/mutex)


{% include comments.html term="Multithreading info Comments" %}