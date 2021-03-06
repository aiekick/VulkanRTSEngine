#include "Precomp.h"
#include "UniformAdapterRegister.h"

#include "ObjectMatricesAdapter.h"
#include "TerrainAdapter.h"
#include "CameraAdapter.h"

UniformAdapterRegister* UniformAdapterRegister::myInstance = nullptr;

UniformAdapterRegister* UniformAdapterRegister::GetInstance()
{
	if (!myInstance)
	{
		myInstance = new UniformAdapterRegister();
		myInstance->RegisterTypes();
	}
	return myInstance;
}

void UniformAdapterRegister::RegisterTypes()
{
	// TODO: add autoregistration macros and get rid of this
	Register<ObjectMatricesAdapter>();
	Register<TerrainAdapter>();
	Register<CameraAdapter>();
}

std::shared_ptr<UniformAdapter> UniformAdapterRegister::GetAdapter
	(const std::string& aName, const GameObject& aGO, const VisualObject& aVO) const
{
	std::unordered_map<std::string, CreationMethod>::const_iterator iter = myCreationMethods.find(aName);
	ASSERT_STR(iter != myCreationMethods.end(), "Requested unregistered adapter!");
	const CreationMethod& createMethod = iter->second;
	return createMethod(aGO, aVO);
}