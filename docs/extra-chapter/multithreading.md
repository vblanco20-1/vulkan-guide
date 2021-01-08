---
layout: default
title: Multithreading for game engines WIP
parent: Extra Chapter
nav_order: 40
---

## Multithreading Overview

For the last 20 years, computers and game consoles have had multiple cores in their CPUs. Over time, the number of cores has increased, with the new consoles having 8 cores with hyperthreading, and PCs getting more and more cores, with things like some ARM servers hitting 80 real cores in a single CPU. Meanwhile, the single-thread performance of CPUs has been relatively stagnant for quite a while, hitting a GHZ barrier of 4-5 gz of clock speed in the CPUs. This means that applications these days need to be programmed for multiple cores to really use the full power of the CPU.

To be able to write good multithreaded code, its important to have an idea of how multicore CPUs are designed and implemented, as it affects directly what the best patterns are.

A multicore CPU generally has multiple "standalone" cores, each of them being a full CPU on its own. Each of the cores can execute an arbitrary program on its own. Generally, each of the cores has its own L1 cache, and thats what the core will use for the data its doing work at the time. If the CPU has hyperthreading/SMT, the CPU will not execute one thread of program instructions, but multiple (often 2). When this happens the inner resources of the CPU (memory units, math units, logic units, caches) will be shared beetween those multiple threads.
In most CPUs, the Level 3 cache, which is much bigger than the L1 cache, will be shared beetween the cores on the CPU. The exact layout of the caches changes depending on the CPU design and architecture, so you can read the information by the manufacturer to know the specifics. Generally L1 is private to each core, and L3 is shared beetween multiple cores.

Each of the cores works on its own data, so there has to be a way to syncronize the data to make sure that things are kept coherent. For that, CPUs implement a system that flags memory locations that each core is working on, and if multiple cores work in the same exact memory location, then circuitry in the CPU makes sure that things are somewhat syncronized. That machinery is slow as the cores need to go to L2 or L3 for everything instead of using their local L1, so when you code something that is multithreaded, you need to make sure that the CPUs generally dont work on the exact same memory locations to avoid that slowdown.

While that memory coherency system works, its very much not reliable as cores can overwrite each others work. For that reason, CPUs implement Atomic operations, which are very specific operations like Add or Compare and Swap that will go through all the cache levels and make sure that the operations happen properly. For example, if you use AtomicAdd operations on a number from multiple cores, the final value will be correct to what each core added. If you just did addition operations without atomics, each core will add the numbers on its own and will likely override each other, so at the end the number wont match up. Atomic operations are the core of all syncronization primitives, and the way you communicate beetween cores. Things like Mutexes are programmed on top of them.

## Ways of using multithreading in game engines.
At first, game engines were completely singlethreaded, and wouldnt use multiple cores at all. Over time, the engines were programmed to use more and more cores, with patterns and architectures that map to that amount of cores.

The first and most classic way of multithreading a game engine is to make multiple threads, and have each of them perform their own task.

For example, you have a Game Thread, which runs all of the gameplay logic and AI. Then you have a Render Thread that handles all the code that deals with rendering, preparing objects to draw and executing graphics commands.

An example we can see of this is Unreal Engine 2, 3, and 4. Unreal Engine 4 is open source, so it can be a good example.

Unreal Engine 4 will have a Game Thread and a Render Thread as main, and then a few others for things such as helpers, audio, or loading. The Game Thread in Unreal Engine will run all of the gameplay logic that developers write in Blueprints and Cpp, and at the end of each frame, it will syncronize the positions and state of the objects in the world with the Render Thread, which will do all of the rendering logic and make sure to display them.

While this approach is very popular and very easy to use, it has the drawback of scaling terribly. You will commonly see that Unreal Engine games struggle scaling past 4 cores, and in consoles the performance is much lower that it could be due to not filling the 8 cores with work. Another issue with this model is that if you have one of the threads have more work than the others, then the entire simulation will wait. In unreal engine 4, the Game THread and Render Thread are synced at each frame, so if either of them is slow, both will be slowed as the run at the same time. A game that has lots of blueprint usage and AI calculations in UE4 will have the Game Thread busy doing work in 1 core, and then every other core in the machine unused.

A common approach of enhancing this architecture is to move it more into a fork/join approach, where you have a main execution thread, and at some points, parts of the work is split between threads. Unreal Engine does this for animation and physics. While the Game Thread is the one in charge of the whole game logic part of the engine, when it reaches the point where it has to do animations, it will split the animations to calculate into small tasks, and distribute those across helper threads in other cores. This way while it still has a main timeline of execution, there are points where it gets extra help from the unused cores. This improves scalability, but its still not good enough as the rest of the frame is still singlethreaded.


Mostly since the ps4 generation of consoles, which have 8 very weak cores, architectures have evolved into trying to make sure all the cores are working on something and doing something useful. A lot of game engines have moved to a Task based system for that purpose. In there, you dont dedicate 1 thread to do one thing, but instead split your work into small sections, and then have multiple threads work on those sections on their own, merging the results after the tasks finish. Unlike the fork-join approach of having one dedicated thread and having it ship off work to helpers, you do everything on the helpers for the most part. Your main timeline of operations is created as a graph of tasks to do, and then those are distributed across cores. A task cant start until all of its predecessor tasks are finished. If a task system is used well, it grants really good scalability as everything automatically distributes to however many cores are available. A great example of this is Doom Eternal, where you can see it smoothly scaling from PCs with 4 cores to PCs with 16 cores.

## In practice.
Cpp since version 11 has a lot of utilities in the standard library that we can use for multithreading. The starter one is `std::thread`. This will wrap around a thread from the operating system. Threads are expensive to create and creating more threads than you have cores will decrease performance due to overhead. Even then, using a `std::thread` to create something like explained above with dedicated threads can be straightforward.

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

Cpp17 added parallelized std::algorithms, which are awesome, but only implemented in Visual Studio for now. You can play around with them if you use VS, but if you want multiplatform, you will need to find alternatives.

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

As stand-in for a task system, we will use `std::async`. std::async creates a lightweight thread, so we can create many of them without too much issue, as they are much cheaper than creating a std::thread. For medium/small lived tasks, they can work nicely. Make sure to check how they run in your platform, as they are very well implemented in Visual Studio, but in GCC/clang their implementation might not be as good. As with the parallel algorithms, you can find a lot of libraries that implement something very similar.

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
    
//syncronize the 3 tasks
particles_task.wait();
animation_task.wait();
physics_task.wait();

```

With this, we are defining a graph of tasks, and their dependencies for execution. We manage to run 3 tasks in parallel at the end, even with 2 of them doing parallel fors, so the threading in here is far superior to the model before. While we still have a very sad fully singlethreaded section in the ObjectUpdate section, at least the other things are reasonably parallelized. 

Having sections of the frame that bottleneck the tasks and have to run in one core only is very common, so what most engines do is that they still have game thread and render thread, but each of them has its own parallel tasks. Hopefully there are enough tasks that can run on their own to fill all of the cores.

While the example here uses async, you really want to use better libraries for this. I personally recomend Taskflow as an amazing library for task execution, but something like  EnkiTS will implement std::async type functionality but far better and with much less overheads.

## Identifying tasks.
While we have been commenting that things like UpdatePhysics() can run overlapped, things are never so simple in practice. 
Identifying what tasks can run at the same time as others is something very hard to do in a project, and in Cpp, its not possible to validate it. If you use rust, this comes automatically thanks to the borrowcheck model. 

A common way to know that a given task can run alongside another one is to "package" the data the task could need, and make sure that the task NEVER accesses any other data outside of it. In the example with game thread and render thread, we have a Renderer class where all the render data is stored, and the game thread never touches that. We also only let the render thread access the gamethread data at a very specific point in the frame where we know the gamethread isnt working on it.

For most tasks, there are places where we just cant guarantee that only one task is working on it at a time, and so we need a way of syncronizing data beetwen the different tasks run at the same time on multiple cores.

## Syncronizing data.
There are a lot of ways of syncronizing the data tasks work on. Generally you really want to minimize the shared data a lot, as its a source of bugs, and can be horrible if not syncronized properly. There are a few patterns that do work quite well that can be used. 

A common one is to have tasks publish messages into queues, and then another task can read it at other point. Lets say that our Particles from above need to be deleted, but the particles are stored in an array, and deleting a particle from that array when the other threads are working on other particles of the same array is a guaranteed way to make the program crash. We could insert the particle IDs into a shared syncronized queue, and after all particles finished their work, delete the particles from that queue.

We are not going to look at the implementation details of the queue. Lets say its a magic queue where you can safely push elements into from multiple cores at once. There are lots of those queues around.

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

