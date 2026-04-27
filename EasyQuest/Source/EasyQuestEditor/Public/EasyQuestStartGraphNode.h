

#pragma once

#include "CoreMinimal.h"
#include "EasyQuestGraphNodeBase.h"
#include "EasyQuestStartGraphNode.generated.h"
/**
 * 
 */
UCLASS()
class UEasyQuestStartGraphNode : public UEasyQuestGraphNodeBase
{
	GENERATED_BODY()

public: // UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override { return FText::FromString("Start"); }
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(FColor::Red); }
	virtual bool CanUserDeleteNode() const override { return false; }

public: // UEasyQuestNodeBase interface

	virtual UEdGraphPin* CreateEasyQuestPin(EEdGraphPinDirection direction, FName name) override;

	virtual EEasyQuestNodeType GetEasyQuestNodeType() const override { return EEasyQuestNodeType::StartNode;}
};
