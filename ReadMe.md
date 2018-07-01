Info
====

This is a mutltithreaded engine project indended as a research in to Vulkan API. The engine features a OpenGL and Vulkan rendering backends with ability to switch them out at will. The engine executes a RTS-battle scenarion until stopped.

Project uses CMake. You'll need VulkanSDK, GLEW, GLM, GLFW, OpenAL, freeglut, Intel's TBB and Bullet3. Tested on Windows, should be runnable on Linux too(no Mac support because of Vulkan).

Intel TBB can be fast found using TBB_ROOT_DIR entry.

Tank model was found on Unity Asset Store (https://www.assetstore.unity3d.com/en/#!/content/46209)
Audio files were found here:
 * https://www.freesound.org/people/joshuaempyre/sounds/251461/
 * http://soundbible.com/1326-Tank-Firing.html
 * http://soundbible.com/1467-Grenade-Explosion.html

Tasks
=====

### Engine
* ~~Reimplement threading model of the engine to be task-based with a dependency tree~~
* Integrate Bullet (with new threading model had to scrap own collision system, so currently there are no collision events firing)
  * Very basic integration done, working on implementing more features
* Integrate DearIMGUI - having access to some runtime info/tools will be nice, can be built upon to make an editor interface
* Clean up readme and move tasks to issues in github

### OpenGL
* Animations
* ~~Add debug drawing~~ (needs refactoring)

### Vulkan
* ~~Fix concurrent GPU mem access~~
* Figure out why can't reuse the swapchain when recreating it on resize
* Animations
* Add debug drawing

Game Video
==========

A minigame implemented using the engine, where the player has to drive around the tank, shoot down enemy tanks while avoiding getting touched by others. If caught, the game ends, and the amount of destroyed tanks is the final score.

<a href="http://www.youtube.com/watch?feature=player_embedded&v=yWnIchIsI7E" target="_blank"><img src="http://img.youtube.com/vi/yWnIchIsI7E/0.jpg" alt="Youtube image" width="240" height="180" border="10" /></a>

Stress Test
===========

A modification of the game logic to stress test the game engine. The engine constantly spawns new tanks for the 2 teams, orders them to move to the other side. The tanks shoot projectiles which destroy enemy vehicles on contact. As visible from the video, rendering artifacts are present - the TBB implementation needs to be improved to avoid this issue. The game scenario simulates about 2600 objects total for a duration of 1 minute. The spawn rate gradually increases from start, and continues to increase throught the test. The objects stabilize at 2600 mark due to nature of the test - it doesn't matter how many are created, they destroy each other as fast.

<a href="http://www.youtube.com/watch?feature=player_embedded&v=7t4nZ0Hbtok" target="_blank"><img src="http://img.youtube.com/vi/7t4nZ0Hbtok/0.jpg" alt="Youtube image" width="240" height="180" border="10" /></a>