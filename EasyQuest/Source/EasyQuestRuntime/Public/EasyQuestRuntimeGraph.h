#pragma once

#include "CoreMinimal.h"
#include "UObject/NameTypes.h"
#include "EasyQuestNodeInfoBase.h"
#include "EasyQuestNodeType.h"
#include "EasyQuestRuntimeGraph.generated.h"

/**
 * 
 */
UCLASS()
class EASYQUESTRUNTIME_API UEasyQuestRuntimePin : public UObject
{
	GENERATED_BODY()

public:
    UPROPERTY()
    FName PinName;

    UPROPERTY()
    FGuid PinId;

    UPROPERTY()
    TArray<UEasyQuestRuntimePin*> Connections;

    UPROPERTY()
    class UEasyQuestRuntimeNode* Parent = nullptr;
};

UCLASS()
class EASYQUESTRUNTIME_API UEasyQuestRuntimeNode : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY()
    EEasyQuestNodeType NodeType = EEasyQuestNodeType::DialogNode;

    UPROPERTY()
    UEasyQuestRuntimePin* InputPin;

    UPROPERTY()
    TArray<UEasyQuestRuntimePin*> OutputPins;

    UPROPERTY()
    FVector2D Position;

    UPROPERTY()
    UEasyQuestNodeInfoBase* NodeInfo = nullptr;
};

UCLASS()
class EASYQUESTRUNTIME_API UEasyQuestRuntimeGraph : public UObject
{
    GENERATED_BODY()

public:

    UPROPERTY();
    TArray<UEasyQuestRuntimeNode*> Nodes;
};