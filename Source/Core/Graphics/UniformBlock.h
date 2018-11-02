#pragma once

#include "GraphicsTypes.h"

class Descriptor;

// A class representing a single block of uniforms for submitting to the GPU.
// Use:
// UniformBlock block(1, descriptor);
// block.SetUniform<glm::vec3>(0, glm::vec3(0.f));
class UniformBlock
{
public:
	UniformBlock(const Descriptor& aDescriptor);
	~UniformBlock();

	template<typename T>
	void SetUniform(uint32_t aSlot, const T& aValue);

	const char* GetData() const { return myData; }
	size_t GetSize() const;

private:
	// TODO: need to add double buffering
	char* myData;
	const Descriptor& myDescriptor;
};

template<typename T>
void UniformBlock::SetUniform(uint32_t aSlot, const T& aValue)
{
	ASSERT_STR(aSlot < myDescriptor.GetUniformCount(), "Either invalid slot was provided, or block wasn't resolved!");
	DEBUG_ONLY(
		size_t slotSize;
		if (aSlot == myDescriptor.GetUniformCount() - 1)
		{
			slotSize = myDescriptor.GetBlockSize() - myDescriptor.GetOffset(aSlot);
		}
		else
		{
			slotSize = myDescriptor.GetOffset(aSlot + 1) - myDescriptor.GetOffset(aSlot);
		}
		ASSERT_STR(slotSize == sizeof(T), "Size mismatch of passed data and slot requested!");
	);
	T* slotPointer = (T*)(myData + myDescriptor.GetOffset(aSlot));
	*slotPointer = aValue;
}