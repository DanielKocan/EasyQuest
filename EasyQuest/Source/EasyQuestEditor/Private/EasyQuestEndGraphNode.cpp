#include "EasyQuestEndGraphNode.h"

FText UEasyQuestEndGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    if (_nodeInfo != nullptr && _nodeInfo->Action != EEasyQuestNodeAction::None) {
        FString result = UEnum::GetDisplayValueAsText(_nodeInfo->Action).ToString();
        if (!_nodeInfo->ActionData.IsEmpty()) {
            FString actionData = _nodeInfo->ActionData;
            if (actionData.Len() > 15) {
                actionData = actionData.Left(15) + TEXT("...");
            }
            result += TEXT(" - ") + actionData;
        }
        return FText::FromString(result);
    }

    return FText::FromString(TEXT("End"));
}

void UEasyQuestEndGraphNode::GetNodeContextMenuActions(UToolMenu* menu, UGraphNodeContextMenuContext* context) const
{
    FToolMenuSection& section = menu->AddSection(TEXT("SectioName"), FText::FromString(TEXT("End Node Actions")));

    UEasyQuestEndGraphNode* node = (UEasyQuestEndGraphNode*)this;
    section.AddMenuEntry(
        TEXT("DeleteEntry"),
        FText::FromString(TEXT("Delete Node")),
        FText::FromString(TEXT("Delete the node")),
        FSlateIcon(TEXT("EasyQuestEditorStyle"), TEXT("EasyQuestEditor.NodeDeleteNodeIcon")), // Has to match style set created in EasyQuestEditor.cpp
        FUIAction(FExecuteAction::CreateLambda(
            [node]() {
                node->GetGraph()->RemoveNode(node);
            }
        ))
    );
}

UEdGraphPin* UEasyQuestEndGraphNode::CreateEasyQuestPin(EEdGraphPinDirection direction, FName name)
{
    FName category = TEXT("Inputs");
    FName subcategory = TEXT("EndPin");

    UEdGraphPin* pin = CreatePin(
        direction,
        category,
        name
    );
    pin->PinType.PinSubCategory = subcategory;

    return pin;
}

UEdGraphPin* UEasyQuestEndGraphNode::CreateDefaultInputPin()
{
    return CreateEasyQuestPin(EEdGraphPinDirection::EGPD_Input, FName(TEXT("Finish")));
}