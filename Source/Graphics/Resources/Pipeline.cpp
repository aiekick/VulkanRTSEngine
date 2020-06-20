#include "Precomp.h"
#include "Pipeline.h"

#include "../Graphics.h"
#include <Core/Resources/Serializer.h>

Pipeline::Pipeline()
	: myType(IPipeline::Type::Graphics)
{
}

Pipeline::Pipeline(Resource::Id anId, const std::string& aPath)
	: Resource(anId, aPath)
	, myType(IPipeline::Type::Graphics)
{
}

void Pipeline::Serialize(Serializer& aSerializer)
{
	uint32_t enumTemp = 0;
	aSerializer.Serialize("type", enumTemp);
	myType = static_cast<IPipeline::Type>(enumTemp);
	ASSERT_STR(myType == IPipeline::Type::Graphics, "Compute pipeline type not supported!");

	aSerializer.Serialize("shaders", myShaders);
	aSerializer.Serialize("descriptors", myDescriptors);
}