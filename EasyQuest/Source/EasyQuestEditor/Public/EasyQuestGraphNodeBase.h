

#pragma once

#include "CoreMinimal.h"
#include "EasyQuestNodeType.h"
#include "EasyQuestNodeInfoBase.h"
#include "EasyQuestGraphNodeBase.generated.h"

/**
 * 
 */
UCLASS()
class UEasyQuestGraphNodeBase : public UEdGraphNode
{
    GENERATED_BODY()

public: // Our interface
    virtual UEdGraphPin* CreateEasyQuestPin(EEdGraphPinDirection direction, FName name) { /* Must be overidden */ return nullptr; };
	virtual UEdGraphPin* CreateDefaultInputPin() { return nullptr; }
	virtual void CreateDefaultOutputPins() { /* Dont do anything here by deafautl */ }

	virtual void InitNodeInfo (UObject* outer) { /* nothing by default*/ }
	virtual void SetNodeInfo(UEasyQuestNodeInfoBase* nodeInfo) { /* None by default */ }
	virtual UEasyQuestNodeInfoBase* GetNodeInfo() const { /* None by default */ return nullptr; }

	virtual EEasyQuestNodeType GetEasyQuestNodeType() const { return EEasyQuestNodeType::Unknown; }
	virtual void OnPropertiesChanged () { /* nothing to do by default*/ }
};
