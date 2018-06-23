#pragma once

#include "TrippleBuffer.h"

class Graphics;
class GameObject;
class Terrain;
struct GLFWwindow;

// a proxy class that handles the render thread
class RenderThread
{
public:
	RenderThread();
	~RenderThread();

	void Init(bool anUseVulkan, const vector<Terrain*>* aTerrainSet);
	void Work();
	bool IsBusy() const { return myHasWorkPending; }
	void RequestSwitch() { myNeedsSwitch = true; }
	GLFWwindow* GetWindow() const;

	Graphics* GetGraphics() { return myGraphics.get(); }
	const Graphics* GetGraphics() const { return myGraphics.get(); }

	void AddRenderable(const GameObject* aGo);

	void InternalLoop();

private:
	const vector<Terrain*>* myTerrains;
	unique_ptr<Graphics> myGraphics;
	bool myIsUsingVulkan;

	// TODO: separate gameobject and renderable
	TrippleBuffer<vector<const GameObject*>> myTrippleRenderables;

	atomic<bool> myNeedsSwitch;
	atomic<bool> myHasWorkPending;
};

