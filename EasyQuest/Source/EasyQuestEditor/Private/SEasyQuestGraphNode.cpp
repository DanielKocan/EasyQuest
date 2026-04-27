#include "SEasyQuestGraphNode.h"
#include "EasyQuestGraphNode.h"
#include "EasyQuestNodeInfo.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/SBoxPanel.h"
#include "SGraphPanel.h"

void SEasyQuestGraphNode::Construct(const FArguments& InArgs, UEasyQuestGraphNode* InNode)
{
    GraphNode = InNode;
    UpdateGraphNode();
}

void SEasyQuestGraphNode::UpdateGraphNode()
{
    // Call the parent implementation first - this handles most of the work
    SGraphNode::UpdateGraphNode();
}

void SEasyQuestGraphNode::CreateStandardPinWidget(UEdGraphPin* Pin)
{
    // Use default pin creation
    SGraphNode::CreateStandardPinWidget(Pin);
}

void SEasyQuestGraphNode::CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox)
{
    // Add Quest Type Dropdown
    /*MainBox->AddSlot()
        .AutoHeight()
        .Padding(5.0f, 2.0f)
        [
            SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .AutoWidth()
                .VAlign(VAlign_Center)
                .Padding(0, 0, 5, 0)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Quest Type:")))
                ]

                + SHorizontalBox::Slot()
                .FillWidth(1.0f)
                .VAlign(VAlign_Center)
                [
                    SNew(SComboButton)
                        .OnGetMenuContent(this, &SEasyQuestGraphNode::GetQuestTypeMenuContent)
                        .ButtonContent()
                        [
                            SNew(STextBlock)
                                .Text(this, &SEasyQuestGraphNode::GetQuestTypeText)
                        ]
                ]
        ];*/

    // Add checkbox BELOW the pins
    MainBox->AddSlot()
        .AutoHeight()
        .Padding(5.0f, 2.0f)
        [
            SNew(SHorizontalBox)

                + SHorizontalBox::Slot()
                .AutoWidth()
                [
                    SNew(SCheckBox)
                        .IsChecked(this, &SEasyQuestGraphNode::GetCheckboxState)
                        .OnCheckStateChanged(this, &SEasyQuestGraphNode::OnCheckboxChanged)
                ]

                + SHorizontalBox::Slot()
                .Padding(5, 0, 0, 0)
                .VAlign(VAlign_Center)
                [
                    SNew(STextBlock)
                        .Text(FText::FromString(TEXT("Completed")))
                ]
        ];
}

ECheckBoxState SEasyQuestGraphNode::GetCheckboxState() const
{
    UEasyQuestGraphNode* QuestNode = Cast<UEasyQuestGraphNode>(GraphNode);
    if (QuestNode)
    {
        UEasyQuestNodeInfo* NodeInfo = QuestNode->GetEasyQuestNodeInfo();
        if (NodeInfo)
        {
            return NodeInfo->isCompleted ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
        }
    }
    return ECheckBoxState::Unchecked;
}

void SEasyQuestGraphNode::OnCheckboxChanged(ECheckBoxState NewState)
{
    UEasyQuestGraphNode* QuestNode = Cast<UEasyQuestGraphNode>(GraphNode);
    if (QuestNode)
    {
        UEasyQuestNodeInfo* NodeInfo = QuestNode->GetEasyQuestNodeInfo();
        if (NodeInfo)
        {
            const FScopedTransaction Transaction(FText::FromString(TEXT("Toggle Completed")));
            NodeInfo->Modify();

            NodeInfo->isCompleted = (NewState == ECheckBoxState::Checked);

            if (QuestNode->GetGraph())
            {
                QuestNode->GetGraph()->NotifyGraphChanged();
            }
        }
    }
}

/*FText SEasyQuestGraphNode::GetQuestTypeText() const
{
    UEasyQuestGraphNode* QuestNode = Cast<UEasyQuestGraphNode>(GraphNode);
    if (QuestNode)
    {
        UEasyQuestNodeInfo* NodeInfo = QuestNode->GetEasyQuestNodeInfo();
        if (NodeInfo)
        {
            return UEnum::GetDisplayValueAsText(NodeInfo->QuestType);
        }
    }
    return FText::FromString(TEXT("None"));
}*/

/*TSharedRef<SWidget> SEasyQuestGraphNode::GetQuestTypeMenuContent()
{
    FMenuBuilder MenuBuilder(true, nullptr);

    // Get the enum
    const UEnum* QuestTypeEnum = StaticEnum<EQuestType>();
    if (QuestTypeEnum)
    {
        // Loop through all enum values
        for (int32 i = 0; i < QuestTypeEnum->NumEnums() - 1; i++)  // -1 to skip the _MAX value
        {
            EQuestType EnumValue = (EQuestType)QuestTypeEnum->GetValueByIndex(i);
            FText DisplayName = QuestTypeEnum->GetDisplayNameTextByIndex(i);

            MenuBuilder.AddMenuEntry(
                DisplayName,
                FText::GetEmpty(),  // No tooltip
                FSlateIcon(),
                FUIAction(FExecuteAction::CreateSP(this, &SEasyQuestGraphNode::OnQuestTypeSelected, EnumValue))
            );
        }
    }

    return MenuBuilder.MakeWidget();
}*/

//void SEasyQuestGraphNode::OnQuestTypeSelected(EQuestType NewType)
//{
//    UEasyQuestGraphNode* QuestNode = Cast<UEasyQuestGraphNode>(GraphNode);
//    if (QuestNode)
//    {
//        UEasyQuestNodeInfo* NodeInfo = QuestNode->GetEasyQuestNodeInfo();
//        if (NodeInfo)
//        {
//            const FScopedTransaction Transaction(FText::FromString(TEXT("Change Quest Type")));
//            NodeInfo->Modify();
//
//            NodeInfo->QuestType = NewType;
//
//            if (QuestNode->GetGraph())
//            {
//                QuestNode->GetGraph()->NotifyGraphChanged();
//            }
//        }
//    }
//}