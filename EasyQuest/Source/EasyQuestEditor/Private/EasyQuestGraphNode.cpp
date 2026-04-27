#include "EasyQuestGraphNode.h"
#include "Framework/Commands/UIAction.h"
#include "ToolMenu.h"
#include "EasyQuestNodeInfo.h"

FText UEasyQuestGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const{
    UEasyQuestNodeInfo* nodeInfo = Cast<UEasyQuestNodeInfo>(_nodeInfo);
    if (nodeInfo->Title.IsEmpty()) { // IF NO TITLE, WE TRUCT FROM DIALOG TEST
        FString dialogTextStr = nodeInfo->QuestDescription.ToString();
        if (dialogTextStr.Len() > 15) {
            dialogTextStr = dialogTextStr.Left(15) + TEXT("...");
        }
        return FText::FromString(dialogTextStr);
    }
    return nodeInfo->Title;
}

void UEasyQuestGraphNode::GetNodeContextMenuActions(UToolMenu* menu, UGraphNodeContextMenuContext* context) const
{
	FToolMenuSection& section = menu->AddSection(TEXT("SectioName"), FText::FromString(TEXT("Custom Node Actions")));

    UEasyQuestGraphNode* node = (UEasyQuestGraphNode*)this;
    section.AddMenuEntry(
        TEXT("AddPinEntry"),
        FText::FromString(TEXT("Add Responce")),
        FText::FromString(TEXT("Creates a new responce")),
        FSlateIcon(TEXT("EasyQuestEditorStyle"), TEXT("EasyQuestEditor.NodeAddPinIcon")), // Has to match style set created in EasyQuestEditor.cpp
        FUIAction(FExecuteAction::CreateLambda(
            [node]() {
                node->GetEasyQuestNodeInfo()->DialogResponses.Add(FText::FromString(TEXT("Response")));
                node->SyncPinsWithResponses();
                node->GetGraph()->NotifyGraphChanged();
                node->GetGraph()->Modify();
            }
        ))
    );

    section.AddMenuEntry(
        TEXT("DeletePinEntry"),
        FText::FromString(TEXT("Delete Pin")),
        FText::FromString(TEXT("Deletes the last pin")),
        FSlateIcon(TEXT("EasyQuestEditorStyle"), TEXT("EasyQuestEditor.NodeDeletePinIcon")), // Has to match style set created in EasyQuestEditor.cpp
        FUIAction(FExecuteAction::CreateLambda(
            [node]() {
                UEdGraphPin* pin = node->GetPinAt(node->Pins.Num() - 1);

                if (pin->Direction != EEdGraphPinDirection::EGPD_Input)
                {
                    UEasyQuestNodeInfo* info = node->GetEasyQuestNodeInfo();
                    info->DialogResponses.RemoveAt(info->DialogResponses.Num() - 1);
                    node->SyncPinsWithResponses();

                    node->GetGraph()->NotifyGraphChanged();
                    node->GetGraph()->Modify();
                }
            }
        ))
    );

    section.AddMenuEntry(
        TEXT("DeleteEntry"),
        FText::FromString(TEXT("Delete Node")),
        FText::FromString(TEXT("Deletes the node")),
        FSlateIcon(TEXT("EasyQuestEditorStyle"), TEXT("EasyQuestEditor.NodeDeleteNodeIcon")), // Has to match style set created in EasyQuestEditor.cpp
        FUIAction(FExecuteAction::CreateLambda(
            [node]() {
                node->GetGraph()->RemoveNode(node);
            }
        ))
    );
}

UEdGraphPin* UEasyQuestGraphNode::CreateDefaultInputPin()
{
    return CreateEasyQuestPin(EEdGraphPinDirection::EGPD_Input, TEXT("Display"));
}

void UEasyQuestGraphNode::CreateDefaultOutputPins()
{
    FString defaultResponce = TEXT("Continue");
    CreateEasyQuestPin(EEdGraphPinDirection::EGPD_Output, FName(defaultResponce));
    GetEasyQuestNodeInfo()->DialogResponses.Add(FText::FromString(defaultResponce));
}

UEdGraphPin* UEasyQuestGraphNode::CreateEasyQuestPin(EEdGraphPinDirection direction, FName name)
{
    FName category = (direction == EEdGraphPinDirection::EGPD_Input) ? TEXT("Inputs") : TEXT("Outputs");
    FName subcategory = TEXT("EasyQuestPin");

    UEdGraphPin* pin = CreatePin(
        direction,
        category,
        name
    );
    pin->PinType.PinSubCategory = subcategory;

    return pin;
}

void UEasyQuestGraphNode::SyncPinsWithResponses()
{
    // Sync the pins on the node with the dialog responses
    // We're going to assume the first pin is always the input pin
    UEasyQuestNodeInfo* nodeInfo = GetEasyQuestNodeInfo();
    int numGraphNodePins = Pins.Num() - 1; 
    int numInfoPins = nodeInfo->DialogResponses.Num();

    while (numGraphNodePins > numInfoPins) {
        RemovePinAt(numGraphNodePins - 1, EEdGraphPinDirection::EGPD_Output);
        numGraphNodePins--;
    }
    while (numInfoPins > numGraphNodePins) {
        CreateEasyQuestPin(
            EEdGraphPinDirection::EGPD_Output,
            FName(nodeInfo->DialogResponses[numGraphNodePins].ToString())
        );
        numGraphNodePins++;
    }

    int index = 1; //syncronised pin with all responces
    for (const FText& option : nodeInfo->DialogResponses) {
        GetPinAt(index)->PinName = FName(option.ToString());
        index++;
    }
}
