#pragma once

#include "CoreMinimal.h"
#include "SGraphNode.h"
#include "EasyQuestNodeInfo.h"

class UEasyQuestGraphNode;

class SEasyQuestGraphNode : public SGraphNode
{
public:
    SLATE_BEGIN_ARGS(SEasyQuestGraphNode) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs, UEasyQuestGraphNode* InNode);

    // Override to add checkbox
    virtual void CreateStandardPinWidget(UEdGraphPin* Pin) override;
    virtual void UpdateGraphNode() override;
    virtual void CreateBelowPinControls(TSharedPtr<SVerticalBox> MainBox) override;

protected:
    // Checkbox state
    ECheckBoxState GetCheckboxState() const;
    void OnCheckboxChanged(ECheckBoxState NewState);

    // Enum dropdown
    // FText GetQuestTypeText() const;
    // TSharedRef<SWidget> GetQuestTypeMenuContent();
};