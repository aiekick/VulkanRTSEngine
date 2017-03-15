#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include "Transform.h"
#include "Graphics.h"
#include "Components\ComponentBase.h"
#include "Components\Renderer.h"

class GameObject
{
public:
	~GameObject();

	void Update(float deltaTime);

	Transform* GetTransform() { return &transf; }

	void SetIndex(size_t newInd) { index = newInd; }
	size_t GetIndex() { return index; }

	// compiler should perform RVO, so please do it
	// https://web.archive.org/web/20130930101140/http://cpp-next.com/archive/2009/08/want-speed-pass-by-value
	auto GetUniforms() const { return uniforms; }

	void AddComponent(ComponentBase *component);
	Renderer* GetRenderer() { return renderer; }

private:
	size_t index;
	
	Transform transf;

	unordered_map<string, Shader::UniformValue> uniforms;

	vector<ComponentBase*> components;
	Renderer *renderer;
};

#endif // !_GAME_OBJECT_H