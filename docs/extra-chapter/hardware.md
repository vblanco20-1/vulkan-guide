---
layout: default
title: CPU Hardware explanations
parent: Extra Chapter
nav_order: 40
---

## Know your hardware
When we program a game engine, or any other kind of high performance system, knowing how the hardware works is critical to be able to do a good performant design. Computers are complex devices, and only getting more complex over time. The way CPUs work has evolved over decades, which requires changing the way software is written for it. 
This article does not try to be fully in-depth, but act as a overview and starting point to the way modern hardware works. In the case you are working on high performance code, knowing this is important to reach maximum performance, and you can find the specific details of these things on the developer site of whatever console you are targetting, or in the documentation websites of your hardware vendor.


## CPU Basics
The heart of a computer is a CPU (Central Processing Unit). Its the thing that will run your programs and operating system. Modern CPUs are marvels of engineering and are highly complex devices with tons of features and tricks to go fast. By going through the timeline of how CPUs have evolved over the time we can understand how those features were implemented and get a general idea of the insanity of a modern CPU and what matters to them when running code. 

A CPU core is a set of logic gates and circuits that performs an operation, then loops and continues on the next. The simplest CPUs can be expressed in barely a few hundreds lines of Verilog (hardware design language), see this [1] small cpu sourcecode . The core is comprised of a set of parts that all work in sync to execute programs.

There is the ALU (Arithmetic Logic Unit), which implements math operations like addition, substraction, and comparing numbers. 
The Register bank is what stores the "working memory" of the processor. You can think of this as the absolute highest performant memory but it's very small, often only enough to store a few numbers. Operations in a CPU work only on registers for the most part, and is connected directly to the ALU as well as other parts of the CPU core.
To store more data, RAM (Random Access Memory) is used, which is also called memory. The CPU can load data from memory into a register, and can also store data from a register into memory. The common flow of a program is that it will load memory, execute some math on it using the ALU, then store the results.
The CPU core is controlled from the Decoder, also known as the Frontend, or many other names. This is the part of a CPU controls the main execution of the core. When running an instruction, this part of the CPU will read the binary code of the instruction, then trigger the different sections of the CPU core to do their work. This part will often have a special register commonly known as PC (Program Counter) which stores the address of the next instruction to load from memory. In the case the program uses branches or jumps, this program counter can be directly addressed and modified. 

Modern cpus will also have multiple of these cores to be able to do more work at once. A set of instructions is generally known as a "thread" of execution, and each of the CPU cores is independent from the others. There are ways of synchronizing information between the cores using atomic instructions, and there are also ways of synchronizing memory across the cores. 
For more in-depth information, check the other article [Multithreading for Game Engines]({{ site.baseurl }}{% link docs/extra-chapter/multithreading.md %}) .

## Pipelining
The amount of work that can be done within a clock cycle depends on how many logic gates and wires the instructions have to go through. It is possible to design a CPU that does 1 instruction per clock, but that makes the clock rate of such a CPU quite slow. To improve that, CPU designers split the instruction execution into different "stages", with each of them doing one part of the instruction, and only needing one section of the core at a time. 

For example, the various stages of executing an instruction roughly follow this pattern
* Fetch: Grabs the next instruction to execute from memory
* Decode: Reads the instruction and sets up the signals to control what the next step will do
* Execute: Do the required operation. This may take multiple clock cycles. CPUs support many kinds of operations, some of the most common ones are:
   * Math operation - Tell the ALU to do math using the registers
   * Comparison - Compare registers
   * Load/Store - Move data between registers and memory
   * Control Flow - Change the PC to a specific instruction in the code
* Writeback: Push the results of the operation into a register or store it in memory

After all that, it begins with the Fetch stage again to continue execution. These stages vary dramatically between CPU designs, and some of them can take more or less time depending on the instruction. See [2] for more details.

Lets look at some very simple example of adding 2 numbers and storing the result.
```
add a b      =  F D E W 
add a c      =  - - - - F D E W
mov a [mem]  =  - - - - - - - - F D W 
```
The 2 additions take 4 clock cycles each, and the mov stores the result in memory, but because it does no math it can skip the execute step so it takes 11 clock cycles to run.

You probably can see that we are activating different parts of the cpu at each clock, and its possible to overlap stages of different instructions. This is known as Pipelining. On the example above, we could start decoding and loading the next instruction as the ALU is working, which would end something like this.
```
add a b      =  F D E W 
add a c      =  - F D E W
mov a [mem]  =  - - F D X W 
```
We had to add a X stage into the memory instruction to  wait for the results of the math operation, by pipelining instructions, we can use more of the cpu at every clock, and get more total work done. In this example here we have gone from 11 clocks into 6 clocks. We also had to make a way to use the `a` register on the second operation before its actually written to use it on the second add sooner.
Pipelining adds considerable amount of complexity to a CPU due to keeping track of a set of rules of what can be overlapped and how, but it also brings big performance gains. Some of the old cpus such as the Gameboy cpu barely had any pipelining due to complexity requirements. That cpu only overlapped the memory load of the next instruction with the current one, but every other stage was not pipelined, so it took 4 clocks per instruction at minimum.
In a modern cpu like a ryzen, we see more than 15 stages in the pipeline, and the cpu even reorders the execution of instructions to improve efficiency.

## Memory caches
Back in the gameboy days, a CPU would run a similar clock rate as the RAM, so you could read and write the RAM within the time of 1 instruction, in barely a few clocks. With modern CPUs, this is no longer the case, as the CPU can execute hundreds of instructions in the same time as it takes to load or store a single value from RAM. 

To mitigate this problem, CPU designers added smaller memories that were closer to the execution unit of the CPU, with the goal that those would be like a secondary, but much faster, RAM. These are what is known as caches. While a modern PC could have 32 gigabytes of ram, it will likely have a L1 (Level 1) cache of 32 kilobytes, which is 1 million times smaller. The CPU memory system is smart, and will load the memory from RAM into cache as its used, and will use smart algorithms to keep the most used memory in the cache. It knows when a value is already in the cache and can be grabbed quickly, versus when it needs to go into the slow system RAM. There are also multiple caches within a CPU, with some of the caches being directly next to one of the cores, and others being a global for all core. This forms a full hierarchy of possible places to load the data. Each of the levels scales in size and speed. As the size grows bigger, its speed falls. This is an intentional tradeoff to work around the limitations physics imposes on CPU designers.

Keeping track of your memory usages and the way the cache works is very often the most important thing you can do to improve performance when programming high speed code. In games and render engines, the working set will often not fit within those 32 kilobytes of L1, and maybe also not on the multiple megabytes that the L3 can hold. To get the best performance, you need to make the best possible use of the memory you load into cache, and try to make things easier for the CPU to predict, as CPUs will start loading memory into the caches before that memory is used if the memory accesses are predictable. Memory loads are generally loaded 64 bytes at a time (known as a "cache line"), so if you access exactly 1 variable from an object and nothing else, you will waste all of the memory around it as empty traffic in the memory system. Finding a way to get the maximum value of those 64 bytes loaded from the cache can bring great wins. The CPU memory predictors generally try to find a pattern of linear iteration, so if your code is doing things on objects in an array with no indirection, its quite likely the CPU will pick up that pattern and speed it up for you. For a presentation on how programming maps to cache usages see this talk on cache effects [3]

## Out of order execution
All of this cache behavior brings a big amount of randomness into how long an instruction takes to load. The same `mov` instruction could take 10 cycles if it was in L1, but could take 200 cycles if it has to access RAM. During that time, the CPU would need to completely stop executing that instruction until the memory arrives. This happened to the PS3 CPU, and was one of its main sources of slowdowns, even though that CPU had a fast clock speed.
To deal with this, CPUs began to execute instructions out of order. This way the pipeline of the CPU can continue doing useful work even if one of the instructions has to wait for some time. Once out of order execution starts being a thing, the CPU can also improve performance by having multiple execution units for the different stages, and extract parallelism by itself. Modern CPUs can enqueue hundreds of instructions and schedule them for execution.

For example, one single CPU core could have 3 separate ALUs. One ALU is running a long operation like a divide, while the other ALUs are doing quick 1-cycle adds. But when doing that we run into a problem, which is that some instructions end at different times, and some instructions also depend on each other. This is handled internally in the CPU through queuing systems, where multiple instructions are read at a time, then after they are decoded they get put into a pending queue until their dependencies are met. Due to instructions using a low amount of registers, the registers required by different queued instructions would constantly conflict with each other, so hardware designers added a "register renamer" to the CPU that connects the low amount of registers used in the code into a much bigger amount of registers that the CPU has to improve instructions overlap. Once a instruction has met its dependencies, it gets added into the different execution units of the CPU, such as a ALU that does adds/sub only, or an ALU that does divides too, or one of multiple memory loaders. 

While instructions run at different amount of clock cycles and they also finish out of order, the CPU keeps track of an output list which will perform things like memory writes and atomic operations fully preserving the order of the code so that it works as written.

Some info can be seen on this wikipedia page [4]


## Branch prediction
With so much deep pipelining and out of order execution inside the CPU, we run into an issue with branches. What happens when the CPU hits a branch but the data is still not known on where to branch? We could stop the whole thing until the data arrives, but that would mean stopping the cpu completely for many clocks, if the branch data depends on a RAM load that might take 200 cycles to arrive. The modern CPUs decide to just decide on the branch direction, and continue executing anyway. If they decide wrong, they will flush the half-done instructions and the pending memory writes, and begin anew. If they guessed correctly all is good and nothing is done. This is known as branch prediction. It tries to predict the future to do educated guesses of where the program flow will go. To do that it keeps storage inside the CPU that records wether a given branch was taken or not last time, and even if there is a pattern like flipping between taken and not taken. The complexity of modern branch predictors is really high, and their specific working details are often closely guarded trade secrets. On something like a i7 or ryzen, the branch predictor hits the correct prediction more than 95% of the time on average. The place where they struggle is when the branch is essentially random for branches that are predictable and stable the predictor will almost always hit it correctly. If the branch is random and is constantly mispredicted, the CPU will stall and have to reset constantly due to the bad predictions, which is often a thing that can slow softare by a significant factor. In some cases, its worth to run both sides of the branch and use a select instruction or a branchless blend as the penalty for misprediction is higher than the calculation. For a video that talks about how to program in a branchless way for performance, "Branchless programming in Cpp" [5] can be a good one. Another great article that explains different types of branch predictor seen in CPUs is this one [14].

## SIMD
All of the above is focused on the core of executing instructions faster. But what if we want to do more work per instruction? Thats where SIMD comes in (Single Instruction, Multiple Data). Due to the needs of getting more work done per instruction, CPUs started adding new instructions that would do multiple operations at once. For example, normally a CPU will add numbers 1 at a time, but if you use AVX512 instruction set (on modern server i7s and last generation Ryzens) they will execute 16 adds within one instruction, providing 16x the work per instruction. 
SIMD generally has considerable limitations due to them being super complicated instructions (due to doing multiple things per instruction) and compilers often really struggle with them. Autovectorization has been bleeding edge research since SIMD instructions were first invented, and even today, trying to rely on autovectorization to maximize performance of a given piece of code is unreliable. To get max performance, programmers often need to use intrinsics (using the SIMD instructions directly in the code) to do their algorithms. There are also languages like ISPC [13] that let you write vectorized code directly.
Over the years, there has been a progression on SIMD instruction sets. We first began with MMX and SSE instruction sets, which were 4-wide, so they ran 4 operations at a time. This was pretty useful for a lot of multimedia and graphics work as you deal with native vec4s very often. For example, the DXMath library (similar to GLM but for directX) uses SSE instructions a lot to accelerate vector and matrix math for game development, which uses vec4s and mat4x4s. As time continued, we then went to AVX instruction set, which worked 8 wide. Now the vectors don't fit the typical vec3s and vec4s used in graphics math, so using them is harder. Also AVX is incompatible with SSE, so the code needs to be rebuilt if you want to use the new instructions and wider execution. AVX 2 was released a few years later, which is still 8x, but it has a few better operations that are useful. 
A few years ago, we got AVX512, which offers 16 wide execution, alongside a very advanced set of operations. Intel decided that gamers don't need it, so on their last generation i7 they removed it. At the time of writing this article, only server-class intel CPUs, and Ryzen cpus support it, which means its a complete nonstarter to be used for game development as according to Steam, as less than 30% of players have CPUs that support AVX512.
On ARM platforms you can find NEON support, which is 4 wide but a bit nicer than the x86 PC SSE equivalent, and on the bleeding edge ARM cores for datacenters you can find SVE which is a vector instruction set that allows variable length vectors so that future CPUs can have even wider SIMD units and current code will still work. 
As there are so many incompatible SIMD feature sets, creating intrinsics manually can be a significant developer effort because the developer would often need to write the code a minimum of 3 times. One for scalar normal code, other for AVX for PC and big consoles, and other for NEON for ARM (Nintendo switch and phones). Or even multiple feature levels for PC alone.
A good video on intrinsics is "The art of SIMD programming"[6] . If you don't want to use intrinsics and prefer a library, xSimd[12] can be a good one, where it uses templates to abstract multiple types of intrinsics.

## Materials
A lot of what is talked here is taken into account by the programming style known as data oriented programming. In that style of programming, developers generally write the code with things like the cache and branch predictors in mind, often placing their data into arrays and processing them in batches. The main book that talks about it is Data Oriented Design[7], and the talks "Data oriented design and C++" [8] and "OOP is dead, long live data oriented design"[9] are classic ones about it. 

For information on specific hardware details, a great source of information is the Agner Fog manuals [10]. The first one is a must read, and the microarchitecture ones talk about a lot of the topics explained here. 
Other source is Wikichip which has good information on the details of different processors. The digital book "Algorithms for Modern Hardware" [15] is also great read that goes in depth on many of these concepts, with specific focus on programming around them and code examples for high performance algorithms.  

## Links
* [1] ["Small Riscv CPU"](https://github.com/YosysHQ/picorv32)
* [2] ["Wikipedia: Instruction Pipelining"](https://en.wikipedia.org/wiki/Instruction_pipelining)
* [3] ["Meeting Cpp: CPU Cache Effects"](https://www.youtube.com/watch?v=mQWuX_KgH00)
* [4] ["Wikipedia: Out of order execution"](https://en.wikipedia.org/wiki/Out-of-order_execution)
* [5] ["CPPCON: Branchless Programming in Cpp"](https://www.youtube.com/watch?v=g-WPhYREFjk)
* [6] ["Performance Summit: The Art of SIMD Programming"](https://www.youtube.com/watch?v=vIRjSdTCIEU)
* [7] ["Data Oriented Design book"](https://www.dataorienteddesign.com/site.php)
* [8] ["CPPCon: Data-Oriented Design and C++"](https://www.youtube.com/watch?v=rX0ItVEVjHc)
* [9] ["CPPCon: OOP Is Dead, Long Live Data-oriented Design"](https://www.youtube.com/watch?v=yy8jQgmhbAU)

* [10] ["Agner fog Optimization manuals"](https://www.agner.org/optimize/)
* [11] ["Wikichip Zen"](https://en.wikichip.org/wiki/amd/microarchitectures/zen)
* [12] ["Github xsimd"](https://github.com/xtensor-stack/xsimd)
* [13] ["ISPC"](https://ispc.github.io/)
* [14] ["Danluu: Branch-prediction"](https://danluu.com/branch-prediction/)
* [15] ["Algorithms for Modern Hardware"](https://en.algorithmica.org/hpc/)
{% include comments.html term="Hardware info Comments" %}