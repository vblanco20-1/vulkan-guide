---
layout: post
title: "Code: Experimenting with ECS in UE4"
excerpt_separator: "<!--more-->"
feature-img: "assets/img/screenshots/spacebattle.jpg"
thumbnail: "assets/img/screenshots/spacebattle.jpg"
categories:
  - Code Experiment
tags:
  - UE4
  - Code
  - ECS
  - Experiment
---
[Video](https://www.youtube.com/watch?v=s8546qGL8ZA)

<iframe src='https://gfycat.com/ifr/EdibleCourteousHorse' frameborder='0' scrolling='no' allowfullscreen width='640' height='328'></iframe>

Inspired by the new Unity ECS system, i decided to try those same techniques with UE4 and C++ instead of Unity and C# . For my experiment, i used the library  [EnTT](https://github.com/skypjack/entt)  to drive the ECS.

<!--more-->

**Entity-System-Component architecture**
========================================


Entities: Just an ID/pointer. Holds Components
Components: Just some data.
Systems: Executes the game code by iterating through entities that have some specific components.


Entity Component System is a different way of architecting game code. In UE4 Actors can hold Components, and both Actors and Components have both behavior (code) and state. In a "pure" ECS architecture, Entities are just a number, they do not have any state or any code, they just link to some components. Components are very small data modules, they do not have code. All the code is done on the Systems, wich manage the different components to have behaviors.

The interesting part of it, is that it can be optimized, REALLY optimized. The components are very small data stored in contiguous arrays, and entities are just indices into this arrays. The library im using has very impressive numbers for iteration performance and creation/deletion. All the memory being this contiguous means that the CPU can vectorize it, and there are very few cache misses.
In general, entities are just a bag of components, and then in the systems you iterate through all the entities in the game that have certain sets of components to execute something.

Example of a very basic "debug draw" system. This system will execute itself once every second, and draw all the entities that have a Position and a DebugSphere component.


```cpp
struct DebugDrawSystem :public System {

    const float UpdateRate = 1;
    float elapsed = 0;

    void update(ECS_Registry &registry, float dt) override
    {
        assert(OwnerActor);
        elapsed -= dt;
        if (elapsed > 0)
        {
            return;
        }
        else
        {
            elapsed = UpdateRate;

            //iterate through every entity with both a Debug Sphere and Position component
            registry.view<FDebugSphere, FPosition>().each([&,dt](auto entity, FDebugSphere & ds, FPosition & pos) {

                DrawDebugSphere(OwnerActor->GetWorld(),pos.pos,ds.radius,12,ds.color,true,UpdateRate);
            });
        }
    }
};
```

This are the Components that are used in that system:



```cpp
struct FDebugSphere {    

    float radius;
    FColor color;
};

struct FPosition {  

    FVector pos;
};
```
    

Thats all it is. The Debug Draw system just asks the ECS Registry for all the entities with Debug Draw and Position, and draws them.


In something like this, "Ticking" entities and their components is so fast its pretty much literally free. Im updating 2500 bullets per frame there, all of them do collision raycasts and each of them finds all spaceships in a radius, and targets them. All of that takes less than 2 miliseconds. It takes more time to update the instanced mesh render component (the ue4 one) than to calculate all the logic for all the bullets.


<blockquote class="imgur-embed-pub" lang="en" data-id="U9fTFIZ"><a href="//imgur.com/U9fTFIZ"></a></blockquote><script async src="//s.imgur.com/min/embed.js" charset="utf-8"></script>

The projectiles and explosions are 100% "pure ECS", and thats why i can spawn and destroy so many without a hitch. They arent seen by unreal engine, they arent an Uobject, and they do not use dynamic memory. They are stored inside the ECS library in contiguous arrays. They are created from an Archetype blueprint. The Archetype is just an AInfo with a lot of "ECS wrapper" components. When i want to spawn a new bullet, i check if there is an archetype for that class, and spawn one. Then from that archetype i spawned i just copy new bullets (or explosions) over and over again. Given that pure ECS entities are just a ID and a bunch of very small components stored in some array somwhere, spawning and destroying bullets is super fast. There is no need to pool anything here.

<blockquote class="imgur-embed-pub" lang="en" data-id="Kl074xA"><a href="//imgur.com/Kl074xA"></a></blockquote><script async src="//s.imgur.com/min/embed.js" charset="utf-8"></script>


The spaceships, on the other hand, are hybrid Actor-ECS. They are actual actors with blueprints, and they have collision. For them, i have created a normal ActorComponent that "links" the normal UE4 actor with its ECS representation. The whole spaceship logic is done on the ECS world, and the Actor blueprint does not have ticking enabled. When a frame starts, the ECS system copies the transform of the Actor into a component, does all the logic, and then copies the transform back into normal unreal engine Actor (using SetActorTransform). This spaceship actors have an "OnKilled" event (wich gets called from the ECS) wich respawns the spaceship.





**Performance**
==============

<blockquote class="imgur-embed-pub" lang="en" data-id="o8fr811"><a href="//imgur.com/o8fr811"></a></blockquote><script async src="//s.imgur.com/min/embed.js" charset="utf-8"></script>

This whole simulation takes less than 9ms to update on the CPU per frame when used in the editor. If its on a "Release" build of the game, it takes 5 milliseconds.

The performance of this is quite impressive, but right now, most of the cpu time is spent interacting with unreal engine. UE4 is not really optimized to move hundreds of Actors, with physics, per frame. Moving the spaceships (SetActorTransform) takes half the CPU time of ALL the ECS. With engine edits it should be possible to have a way of "mass updating" hundreds of actors for a much lower overhead. The second most costly thing is the Instance Rendering. All the bullets and explosions are just instanced meshes, but unreal engine also cant update a bunch of instanced meshes at once, so i have to call "SetInstanceTransform" over and over again, and every time it causes overhead. Again, with engine edits it should be possible to batch update the instances by just sending an array of all the transform. I look forward to Niagara becouse it could be an easier way of rendering ECS actors by abusing how particle systems instance things.







**Boid Simulation**
===================

<blockquote class="imgur-embed-pub" lang="en" data-id="klhF1m6"><a href="//imgur.com/klhF1m6"></a></blockquote><script async src="//s.imgur.com/min/embed.js" charset="utf-8"></script>


The 3rd most costly thing in this project is the behavior for the bullets and the spaceships. The spaceships all have crowd separation, and the bullets have a range and move towards any enemy (other faction) that comes near. For this, i made an acceleration structure based on an sparse tile map. Essentially i have a TMap for TileLocation and TileData, and when the game updates, every entity that has a "GridMap" component gets sent into the data structure. One would think creating this data structure takes time, but it doesnt. I add the 400 spaceships into this structure every frame, and it takes 0.01 miliseconds.

When i need to "find all entities within X units" of another entity (for the homing bullets and avoidance on the spaceships), i just query the tile map for the nearby tiles and the objects inside them. This decreases the number of candidate entities a lot. Given the incredibly huge amount of entites that need to be checked, this took 7 miliseconds to update on my first implementation. If it didnt use the tile system, it would take a ridiculous amount of time.

To optimize it, i decided to multithread it.

One of the most interesting things about ECS architecture, is that all the Systems are essentially "For All Entities with Components A and B, do logic". This makes them an easy candidate to parallelize. Unity does this, and they created their ECS architecture in a way that it integrates directly with their new Job system.

Luckly, UE4 also has a job system, and there are a few interesting things on it. As most of the Systems are doing logic in their own world, separate from unreal, they are very good candidates to parallelize. For the homing behavior on the bullets, and for the separation behavior on the spaceships, i just used ParallelFor to execute it. First i "asked" the ECS library for all the entities with the components i wanted (Spaceship,Position,Velocity) for example. And then stored all of them into an array. Then i just execute the parallel for in that array. The tile map is read-only so its safe to read from multiple threads. Multithreading the boid simulation improved the calculation from 7 millseconds into less than 2. (Ryzen).




```cpp   
    //ask the ECS registry for how many spaceships there are
    int nShips = registry.raw<FSpaceship>().size();

    SpaceshipArray.Reset(nShips);
        //iterate through all spaceships with some components, and store them in an array
    registry.view<FSpaceship, FPosition, FVelocity, FFaction>().each([&, dt](auto entity, FSpaceship & proj, FPosition & pos, FVelocity & vel, FFaction & faction) {

        SpaceshipData Projectile;
        Projectile.faction = &faction;
        Projectile.pos = &pos;
        Projectile.vel = &vel;
        Projectile.ship = &proj;
        SpaceshipArray.Add(Projectile);
    });

    //Update the movmenet for each spaceship in parallel
    ParallelFor(SpaceshipArray.Num(), [&](int32 Index)
    {
        SpaceshipData data = SpaceshipArray[Index];
        const float shipCheckRadius = 1000;

        //grab nearby entities from the gridmap
        Foreach_EntitiesInRadius(shipCheckRadius, data.pos->pos, [&](GridItem item) {

            if (item.Faction == data.faction->faction)
            {
                const FVector TestPosition = item.Position;
                const float DistSquared = FVector::DistSquared(TestPosition, data.pos->pos);
                const float AvoidanceDistance = shipCheckRadius * shipCheckRadius;
                const float DistStrenght = FMath::Clamp(1.0 - (DistSquared / (AvoidanceDistance)), 0.1, 1.0) * dt;
                const FVector AvoidanceDirection = data.pos->pos - TestPosition;

                data.vel->Add(AvoidanceDirection.GetSafeNormal() * data.ship->AvoidanceStrenght*DistStrenght);
            }
        });

        //finish the speed and clamp it to max velocity.
        FVector ToTarget = data.ship->TargetMoveLocation - data.pos->pos;
        ToTarget.Normalize();

        data.vel->Add(ToTarget * 500 * dt);
        data.vel->vel = data.vel->vel.GetClampedToMaxSize(data.ship->MaxVelocity);
    });
```
    
    


**Benefits**
============


C++ architecture is very simplified. You only need to deal with the specific Systems for each thing. This makes mantainability extremelly easy, and you can follow the logic of the whole game with easy.
This system is *extremelly fast* . This is orders of magnitude faster than normal unreal engine components and actors. This can let you increase the amount of objects in the world without issue.
The components are very modular. The fact that they dont have logic by themselves means you can attach any component to anything. Components shouldnt be as big as the components you create for normal ue4, but small composable modules that create behavior. For example i can put a "Linear Movement" component on anything. If that anything also has a Velocity and a Position component it now moves in a straight line. If i want to make it have gravity, i give it a Gravity component.
Its easy to parallelize and optimize "after the fact". As all the logic is self-contained in the systems update loops, you just need to look at a system that takes more time than it should, and improve it. If your system just takes too much time, you can just paralelize it with a Parallel For with ease. You can even execute several systems at the same time as long as they are editing different kinds of components, and they arent adding/removing entities.


**Downsides**
=============


As explained, the biggest issue is the interaction with actual unreal engine code, wich is not designed for this kind of "mass updating". With some engine tweaks this system could be twice as fast or more.
Another issue is the fact that this is a C++ thing. While you can interface events, how do you interface events of a bullet when that bullet isnt even a "thing" for unreal engine. On the Hybrid actors such as the spaceships, its easy to give the wrapper components a event dispatcher or delegate to fire. But the hybrid actors are much more costly performance wise due to the "copy back and forth" from the Actor into the Entity, and then back again. It is still a net gain if you have actors that love to tick, as "ticking" ECS things is the default, and its pretty much free in performance.
Last, this is a pure C++ system, so it does not support replication. Luckily, this is also extremelly easy to write networked code for if needed.

Still, my new project is another VR game, where i need the absolute highest performance, and i think i can do the engine tweaks for "mass updating" myself, decreasing the cost of Hybrid actors by a lot, so im going to use this systems for the new game.
