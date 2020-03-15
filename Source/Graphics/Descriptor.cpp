#include "Precomp.h"
#include "Descriptor.h"

#include <nlohmann/json.hpp>

bool Descriptor::FromJSON(const nlohmann::json& jsonHandle, Descriptor& aDesc)
{
	using json = nlohmann::json;

	const json& adapterHandle = jsonHandle["adapter"];
	if (!adapterHandle.is_string())
	{
		return false;
	}

	std::string adapterName = adapterHandle.get<std::string>();

	const json& uniformArrayHandle = jsonHandle["members"];
	if (!uniformArrayHandle.is_array())
	{
		return false;
	}
	size_t uniformCount = uniformArrayHandle.size();
	ASSERT_STR(uniformCount < std::numeric_limits<uint32_t>::max(), "Uniform index doesn't fit - will overlap!");

	aDesc = Descriptor(adapterName);
	
	for (size_t i = 0; i < uniformCount; i++)
	{
		// TODO: once uniform types stabilize, need to remove string parsing
		std::string uniformTypeName = uniformArrayHandle.at(i);
		if (uniformTypeName == "Mat4")
		{
			aDesc.SetUniformType(static_cast<uint32_t>(i), UniformType::Mat4);
		}
		else if (uniformTypeName == "Vec4")
		{
			aDesc.SetUniformType(static_cast<uint32_t>(i), UniformType::Vec4);
		}
		else if (uniformTypeName == "Vec3")
		{
			aDesc.SetUniformType(static_cast<uint32_t>(i), UniformType::Vec3);
		}
		else if (uniformTypeName == "Vec2")
		{
			aDesc.SetUniformType(static_cast<uint32_t>(i), UniformType::Vec2);
		}
		else if (uniformTypeName == "Int")
		{
			aDesc.SetUniformType(static_cast<uint32_t>(i), UniformType::Int);
		}
		else if (uniformTypeName == "Float")
		{
			aDesc.SetUniformType(static_cast<uint32_t>(i), UniformType::Float);
		}
		else
		{
			ASSERT_STR(false, "Not supported!");
			return false;
		}
	}
	aDesc.RecomputeSize();

	return true;
}

Descriptor::Descriptor()
	: Descriptor("")
{
}

Descriptor::Descriptor(const std::string& anAdapterName)
	: myTotalSize(0)
	, myUniformAdapter(anAdapterName)
{
}

void Descriptor::SetUniformType(uint32_t aSlot, UniformType aType)
{
	if (aSlot >= myTypes.size())
	{
		myTypes.resize(aSlot + 1);
	}
	myTypes[aSlot] = aType;
}

void Descriptor::RecomputeSize()
{
	const size_t size = myTypes.size();
	myOffsets.resize(size);

	myTotalSize = 0;
	for (size_t i = 0; i < size; i++)
	{
		size_t basicAlignment;
		size_t elemSize;
		switch (myTypes[i])
		{
		case UniformType::Float:
		case UniformType::Int:
			basicAlignment = sizeof(int);
			elemSize = sizeof(int);
			break;
		case UniformType::Vec2:
			basicAlignment = sizeof(glm::vec2);
			elemSize = sizeof(glm::vec2);
			break;
		case UniformType::Vec3:
			// std140 - https://www.khronos.org/registry/OpenGL/specs/gl/glspec45.core.pdf#page=159
			// If the member is a three-component vector with components consuming N
			// basic machine units, the base alignment is 4N
			// Despite that, glsl compiler can merge the next single unit
			// to this 3, to avoid wasting MUs.
			if (i + 1 < size
				&& (myTypes[i + 1] == UniformType::Float
					|| myTypes[i + 1] == UniformType::Int))
			{
				basicAlignment = sizeof(glm::vec4);
				elemSize = sizeof(glm::vec3);
			}
			else
			{
				basicAlignment = sizeof(glm::vec4);
				elemSize = sizeof(glm::vec4);
			}
			break;
		case UniformType::Vec4:
			basicAlignment = sizeof(glm::vec4);
			elemSize = sizeof(glm::vec4);
			break;
		case UniformType::Mat4:
			basicAlignment = sizeof(glm::vec4);
			elemSize = sizeof(glm::mat4);
			break;
		default:
			ASSERT_STR(false, "Unrecognized Uniform Type found!");
		}
		// myTotalSize has the aligned offset from the previous iter
		// plus the size of the new element.
		// calculate the adjusted size (with padding), so that we can
		// retain alignment required for new element
		size_t remainder = myTotalSize % basicAlignment;
		if (remainder)
		{
			myTotalSize -= remainder;
			myTotalSize += basicAlignment;
		}
		
		ASSERT_STR(myTotalSize < std::numeric_limits<uint32_t>::max(), "Descriptor size exceeding allowed!");
		myOffsets[i] = static_cast<uint32_t>(myTotalSize);
		myTotalSize += elemSize;
	}

	// since it's a struct, per std140 rules it has to be 16-aligned
	constexpr size_t kStructAlignment = 16;
	size_t remainder = myTotalSize % kStructAlignment;
	if (remainder)
	{
		myTotalSize -= remainder;
		myTotalSize += kStructAlignment;
	}
}

size_t Descriptor::GetSlotSize(uint32_t aSlot) const
{
	switch (myTypes[aSlot])
	{
	case UniformType::Float:
	case UniformType::Int:
		return sizeof(int);
		break;
	case UniformType::Vec2:
		return sizeof(glm::vec2);
		break;
	case UniformType::Vec3:
		return sizeof(glm::vec3);
		break;
	case UniformType::Vec4:
		return sizeof(glm::vec4);
		break;
	case UniformType::Mat4:
		return sizeof(glm::mat4);
		break;
	default:
		ASSERT_STR(false, "Unrecognized Uniform Type found!");
		return 0;
	}
}