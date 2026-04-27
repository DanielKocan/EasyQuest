#include "EasyQuestFactory.h"
#include "EasyQuestAsset.h"

UEasyQuestFactory::UEasyQuestFactory(const FObjectInitializer& objectInitializer) : Super(objectInitializer)
{
	SupportedClass = UEasyQuestAsset::StaticClass();
}

UObject* UEasyQuestFactory::FactoryCreateNew(UClass* uclass, UObject* inParent, FName name, EObjectFlags flags, UObject* context, FFeedbackContext* warn)
{
	UEasyQuestAsset* asset = NewObject<UEasyQuestAsset>(inParent, name, flags);
	return asset;
}

bool UEasyQuestFactory::CanCreateNew() const
{
	// TODO: Pottentially to check if there is already a class with this name and prevent to create a new one?
	return true;
}
