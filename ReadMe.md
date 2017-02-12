Info
====

This is a mutltithreaded engine project indended as a research in to Vulkan API. The engine features a OpenGL and Vulkan rendering backends with ability to switch them out at will. The engine executes a RTS-battle scenarion until stopped.

Tasks
=====

# OpenGL
* When shutting down make sure to wait on threads to reach the Waiting phase
* Add resource cleanup on unload
* Test the center calculation logic for meshes on other, simpler objects
* Improve RenderJob to store the required uniforms

# Vulkan
* Get it up-to-date with the current execution model
* Add texture support
* Add vertex and uniform buffering