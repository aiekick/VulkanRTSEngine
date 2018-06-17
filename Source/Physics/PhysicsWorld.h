#pragma once

class PhysicsEntity;
class btBroadphaseInterface;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btSequentialImpulseConstraintSolver;
class btDiscreteDynamicsWorld;

// TODO: implement a command queue for phys world and entity commands
class PhysicsWorld
{
public:
	PhysicsWorld();
	~PhysicsWorld();

	void AddEntity(PhysicsEntity* anEntity);
	void RemoveEntity(PhysicsEntity* anEntity);

	void Simulate(float aDeltaTime);

private:
	const int MaxSteps = 4;
	const float FixedStepLength = 1.f / 30.f;

	// TODO: check out Bullet3
	btBroadphaseInterface* myBroadphase;
	btDefaultCollisionConfiguration* myConfiguration;
	btCollisionDispatcher* myDispatcher;
	btSequentialImpulseConstraintSolver* mySolver; // for now using default one, should try ODE quickstep solver later 
	btDiscreteDynamicsWorld* myWorld;
};