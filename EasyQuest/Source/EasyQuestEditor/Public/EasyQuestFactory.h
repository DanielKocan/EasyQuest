#pragma once

#include "CoreMinimal.h"
#include "EasyQuestFactory.generated.h"

/**
 We need this class for EasyQuestAction, since it want to create a custom class and we need to specify it here
 Basically: Creates new quest assets when you right-click in Content Browser
 **/
UCLASS()
class UEasyQuestFactory : public UFactory
{
	GENERATED_BODY()

public:
	UEasyQuestFactory(const FObjectInitializer& objectInitializer);

public:

	virtual UObject* FactoryCreateNew(UClass* uclass, UObject* inParent, FName name, EObjectFlags flags, UObject* context, FFeedbackContext* warn);
	virtual bool CanCreateNew() const override;
};
