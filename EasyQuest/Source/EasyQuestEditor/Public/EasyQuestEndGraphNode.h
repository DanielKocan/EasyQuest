#pragma once

#include "EdGraph/EdGraphNode.h"
#include "EasyQuestEndNodeInfo.h"
#include "EasyQuestNodeType.h"
#include "EasyQuestGraphNodeBase.h"
#include "EasyQuestEndGraphNode.generated.h"

UCLASS()
class UEasyQuestEndGraphNode : public UEasyQuestGraphNodeBase
{
	GENERATED_BODY()

public: // UEdGraphNode
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override { return FLinearColor(FColor::Blue); }
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual void GetNodeContextMenuActions(class UToolMenu* menu, class UGraphNodeContextMenuContext* context) const override;

public: // EasyQuestGraphNodeBase intercace
	virtual UEdGraphPin* CreateEasyQuestPin(EEdGraphPinDirection direction, FName name) override;
	virtual UEdGraphPin* CreateDefaultInputPin() override;

	virtual void InitNodeInfo(UObject* outer)  override { _nodeInfo = NewObject<UEasyQuestEndNodeInfo>(outer); }
	virtual void SetNodeInfo(UEasyQuestNodeInfoBase* nodeInfo) override { _nodeInfo = Cast<UEasyQuestEndNodeInfo>(nodeInfo); }
	virtual UEasyQuestNodeInfoBase* GetNodeInfo() const override { return _nodeInfo; }

	virtual EEasyQuestNodeType GetEasyQuestNodeType() const override { return EEasyQuestNodeType::EndNode; }
	virtual void OnPropertiesChanged() { Modify(); }

protected:
	UPROPERTY()
	class UEasyQuestEndNodeInfo* _nodeInfo = nullptr;
};
