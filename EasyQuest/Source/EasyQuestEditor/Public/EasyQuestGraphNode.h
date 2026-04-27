#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EasyQuestGraphNodeBase.h"
#include "EasyQuestNodeInfo.h"
#include "EasyQuestGraphNode.generated.h"

UCLASS()
class UEasyQuestGraphNode : public UEasyQuestGraphNodeBase
{
	GENERATED_BODY()

public: // UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(FColor::Green); }
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual void GetNodeContextMenuActions(class UToolMenu* menu, class UGraphNodeContextMenuContext* context) const override;

	virtual UEdGraphPin* CreateDefaultInputPin() override;
	virtual void CreateDefaultOutputPins() override;

	virtual EEasyQuestNodeType GetEasyQuestNodeType() const override { return EEasyQuestNodeType::DialogNode; }
	virtual void OnPropertiesChanged() override { SyncPinsWithResponses(); }

public: // Our interface

	virtual UEdGraphPin* CreateEasyQuestPin(EEdGraphPinDirection direction, FName name) override;
	void SyncPinsWithResponses();

	virtual void InitNodeInfo(UObject* outer) override { _nodeInfo = NewObject<UEasyQuestNodeInfo>(outer); }
	virtual void SetNodeInfo(UEasyQuestNodeInfoBase* nodeInfo) override { _nodeInfo = Cast<UEasyQuestNodeInfo>(nodeInfo); }
	virtual UEasyQuestNodeInfoBase* GetNodeInfo() const override { return _nodeInfo; }
	virtual UEasyQuestNodeInfo* GetEasyQuestNodeInfo() const { return _nodeInfo; }

protected:
	UPROPERTY()
	class UEasyQuestNodeInfo* _nodeInfo = nullptr;
};
