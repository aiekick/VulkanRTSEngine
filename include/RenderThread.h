#pragma once

#include "Terrain.h"
#include "TrippleBuffer.h"

class Graphics;
class GameObject;

// a proxy class that handles the render thread
class RenderThread
{
public:
	RenderThread();
	~RenderThread();

	void Init(bool useVulkan, const vector<Terrain>* aTerrainSet);
	void Work();
	bool IsBusy() const { return workPending; }

	Graphics* GetGraphicsRaw() { return graphics.get(); }
	const Graphics* GetGraphics() const { return graphics.get(); }

	void AddRenderable(const GameObject* go);

	void InternalLoop();
	bool HasWork() const { return workPending; }

private:
	const vector<Terrain>* terrains;
	unique_ptr<Graphics> graphics;
	bool usingVulkan;

	// TODO: separate gameobject and renderable
	// TODO: start using TrippleBuffer
	TrippleBuffer<const GameObject*> myTrippleRenderables;

	atomic<bool> needsSwitch;
	atomic<bool> workPending;
};
