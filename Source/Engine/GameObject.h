#pragma once

#include "Graphics.h"
#include "Transform.h"
#include "UID.h"

class Renderer;
class ComponentBase;

class GameObject
{
public:
	GameObject(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);
	~GameObject();

	void Update(float deltaTime);

	const Transform& GetTransform() const { return transf; }
	Transform& GetTransform() { return transf; }

	const glm::mat4 GetMatrix() const { return myCurrentMat; }
	// This is model's center
	glm::vec3 GetCenter() const { return center; }
	float GetRadius() const;

	const UID& GetUID() const { return myUID; }

	// compiler should perform RVO, so please do it
	// https://web.archive.org/web/20130930101140/http://cpp-next.com/archive/2009/08/want-speed-pass-by-value
	auto GetUniforms() const { return uniforms; }

	void AddComponent(ComponentBase *component);
	ComponentBase* GetComponent(int type) const;
	Renderer* GetRenderer() const { return renderer; }

	void SetCollisionsEnabled(bool val) { collisionsEnabled = val; }
	bool GetCollisionsEnabled() const { return collisionsEnabled; }
	void CollidedWithTerrain();
	void CollidedWithGO(GameObject *go);
	void PreCollision();

	bool IsDead() const { return dead; }
	void Die();

private:
	UID myUID;

	size_t index = numeric_limits<size_t>::max();
	bool dead = false;
	Transform transf;
	glm::mat4 myCurrentMat;
	glm::vec3 center;

	unordered_map<string, Shader::UniformValue> uniforms;

	bool collisionsEnabled = true;
	bool collidedWithTerrain = false;
	tbb::concurrent_unordered_set<GameObject*> objsCollidedWith;

	vector<ComponentBase*> components;
	Renderer* renderer = nullptr;
};