#include "EasyQuestGraphSchema.h"
#include "EasyQuestNodeInfo.h"
#include "EasyQuestStartGraphNode.h"
#include "EasyQuestEndGraphNode.h"
#include "EasyQuestGraphNode.h"

void UEasyQuestGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& contextMenuBuilder) const
{
    TSharedPtr<FNewNodeAction> newNodeAction(
        new FNewNodeAction(
            UEasyQuestGraphNode::StaticClass(),
            FText::FromString(TEXT("Nodes")),
            FText::FromString(TEXT("New EasyQuest node")), // label in menu
            FText::FromString(TEXT("Makes a new EasyQuest node")), // tooltip
            0
        )
    );

    TSharedPtr<FNewNodeAction> newEndNodeAction(
        new FNewNodeAction(
            UEasyQuestEndGraphNode::StaticClass(),
            FText::FromString(TEXT("Nodes")),
            FText::FromString(TEXT("New End node")), // label in menu
            FText::FromString(TEXT("Makes a new end node")), // tooltip
            0
        )
    );

    contextMenuBuilder.AddAction(newNodeAction);
    contextMenuBuilder.AddAction(newEndNodeAction);
}

const FPinConnectionResponse UEasyQuestGraphSchema::CanCreateConnection(const UEdGraphPin* a, const UEdGraphPin* b) const
{
    // Neither can be nullptr
    if (a == nullptr || b == nullptr) {
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Need 2 pins"));
    }

    // Make sure the directions are different (no inputs to inputs or outputs to outputs)
    if (a->Direction == b->Direction) {
        return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Inputs can only connect to outputs"));
    }

    // Break others only on the OUTPUT side (one output goes to one place), but allow multiple on input
    if (a->Direction == EEdGraphPinDirection::EGPD_Output || b->Direction == EEdGraphPinDirection::EGPD_Output)
    {
        return FPinConnectionResponse(CONNECT_RESPONSE_BREAK_OTHERS_A, TEXT(""));
    }
    return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
}

void UEasyQuestGraphSchema::CreateDefaultNodesForGraph(UEdGraph& graph) const
{
    UEasyQuestStartGraphNode* startNode = NewObject<UEasyQuestStartGraphNode>(&graph);
    startNode->CreateNewGuid();
    startNode->NodePosX = 0;
    startNode->NodePosY = 0;

    startNode->CreateEasyQuestPin(EEdGraphPinDirection::EGPD_Output, FName(TEXT("Start")));

    graph.AddNode(startNode, true, true);
    graph.Modify();
}

// Hapned when you right click and create node
UEdGraphNode* FNewNodeAction::PerformAction(UEdGraph* parentGraph, UEdGraphPin* fromPin, const FVector2D location, bool bSelectNewNode)
{
    UEasyQuestGraphNodeBase* result = NewObject<UEasyQuestGraphNodeBase>(parentGraph, _classTemplate);
    result->CreateNewGuid();
    result->NodePosX = location.X;
    result->NodePosY = location.Y;
    result->InitNodeInfo(result);

    UEdGraphPin* inputPin = result->CreateDefaultInputPin();
    result->CreateDefaultOutputPins();

    if (fromPin != nullptr)
    {
        result->GetSchema()->TryCreateConnection(fromPin, inputPin);
    }

    parentGraph->Modify();
    parentGraph->AddNode(result, true, true);

    return result;
}
