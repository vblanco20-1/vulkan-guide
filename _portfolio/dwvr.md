---
layout: post
title: DWVR
feature-img: "assets/img/portfolio/dwvr-big.jpg"
img: "assets/img/portfolio/dwvr.png"
date: 27 September 2015
tags: [Game, VR, Consoles]
---

DWVR game project, Released for [Oculus](https://www.oculus.com/experiences/rift/1171112789643848/), [Steam](http://store.steampowered.com/app/520750/DWVR/), and [PSVR](https://store.playstation.com/en-us/product/UP3089-CUSA09805_00-000000000000DWVR). Created on a shoestring budget over 3 months (PC early access version) and 6 months to port to PSVR and finish the game.

This game was created with the assets from Deathwave, wich i decided to reuse. Thats the reason its called DWVR. It doesnt have any code from the older deathwave, as eventually everything got replaced.

After releasing VRMultigames, I heard of Playstation Spain running the PS Awards, wich was a contest for very small amateur indie developers. I decided to join this to pave the way to become a PS4 developer and release games there.

I only had 2 weeks to apply to the awards, so i decided to take my favourite VRMultigames prototype (Last Stand), and merge it with the assets of Deathwave. This is the prototype i had 4 days after i decided to do it.

<iframe width="700" height="315" src="https://www.youtube.com/embed/vgZWyoVuwOg" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>



I used this prototype and a design document i improvised on the spot to apply for the Playstation Awards. It worked and i became a semi-finalist. Sadly, i did not reach the finalist stage. One week after i dropped from the contest, i released the game on early access, as i had it prepared. From first prototype to early access release it took 3 months. I released the game on both Steam and Oculus, but i missed the Oculus Touch launch sale by 1 week, wich was an absolute shame. The game made around 8000 dollars on this PC release.

After the PC release, i became a PS4 developer (thanks to the contacts i got from the contest), and used the money i earned for PC to port the game to PSVR. Given the performance charasteristics of the PS4, i had to eventually rewrite most of the game. After rewriting the enemy code to use less collisions to avoid physics costs, and pooling everything that could be pooled, and made sure the graphics of the game are LITERALLY ZERO dynamic lights in the whole game, i ported the game to PSVR. This took around 2-3 months. The rest of the time until release was spent growing the game and adding more features. To profile and benchmark the game, i created a few automated "Chaos monkey" AIs, wich essentially are very simple bots attached to the same player controls. It simulates the player jumping around randomly around the map, and shooting at everything close by. I left this ones running in a loop for hours, and then see the performance logs and if it crashed.

<iframe width="700" height="315" src="https://www.youtube.com/embed/nI0Wr-4Grc0" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>



The reception of the game on PSVR was positive, but there were some hideous day 1 bugs, wich i had to scramble to patch. These bugs were all caused by my lacking QA process, as i had to debug everything myself. While the chaos AIs made sure that the game doesnt crash and actually runs at a proper performance, there were other more subtle bugs that i completely missed.

**Credits**: 

Lydia Diaz - Character models, non firearm weapon models.

Victor Blanco - All code, All game design, Animation, FX

Freelancers - Sound effects, Environment design

Marketplace - Some animations, FX (heavily modified for the game), Environment assets, firearm assets. 

