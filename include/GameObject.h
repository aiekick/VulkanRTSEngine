#ifndef _GAME_OBJECT_H
#define _GAME_OBJECT_H

#include "Graphics.h"

class GameObject
{
public:
	GameObject() : pos(vec3(0, 0, 0)), size(vec3(1, 1, 1)) {}
	~GameObject() {}

	void Update(float deltaTime);

	void SetModel(ModelId newModel) { model = newModel; }
	ModelId GetModel() const { return model; }

	void SetShader(ShaderId newShader) { shader = newShader; }
	const ShaderId GetShader() const { return shader; }

	void SetTexture(TextureId newTexture) { texture = newTexture; }
	TextureId GetTexture() const { return texture; }

	vec3 GetPos() const { return pos; }
	void SetPos(vec3 pPos) { pos = pPos; modelDirty = true; }
	void Move(vec3 delta) { pos += delta; modelDirty = true; }

	vec3 GetRotation() const { return rotation; }
	void SetRotation(vec3 pRotation) { rotation = pRotation; modelDirty = true; }
	void AddRotation(vec3 delta) { rotation += delta; modelDirty = true; }

	vec3 GetScale() const { return size; }
	void SetScale(vec3 pScale) { size = pScale; modelDirty = true; }
	void AddScale(vec3 delta) { size += delta; modelDirty = true; }

	mat4 GetModelMatrix() { if (modelDirty) UpdateMatrix(); return modelMatrix; }

	void SetIndex(size_t newInd) { index = newInd; }
	size_t GetIndex() { return index; }

	// compiler should perform RVO, so please do it
	// https://web.archive.org/web/20130930101140/http://cpp-next.com/archive/2009/08/want-speed-pass-by-value
	auto GetUniforms() const { return uniforms; }

private:
	size_t index;
	ModelId model;
	ShaderId shader;
	TextureId texture;

	vec3 pos, rotation, size;
	mat4 modelMatrix;
	bool modelDirty = true;
	void UpdateMatrix();

	unordered_map<string, Shader::UniformValue> uniforms;
};

#endif // !_GAME_OBJECT_H