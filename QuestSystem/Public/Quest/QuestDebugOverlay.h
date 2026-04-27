#pragma once

#include "CoreMinimal.h"
#include "Quest/BaseQuest.h"
#include "Widgets/SLeafWidget.h"

/**
 * SQuestDebugOverlay
 *
 * Self-contained Slate widget — same pattern as SGameDataTrackerDebugOverlay.
 * Shows all quests grouped by state in the top-left corner.
 *
 * QuestManager only calls Start() and Stop().
 * All logic (timer, data fetching, drawing) lives here.
 */
class SQuestDebugOverlay : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SQuestDebugOverlay) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual ~SQuestDebugOverlay();

    /** Called by QuestManager — adds to viewport and starts refresh timer */
    void Start(UGameInstance* InGameInstance);

    /** Called by QuestManager — removes from viewport and clears timer */
    void Stop();

    // SLeafWidget interface
    virtual int32 OnPaint(
        const FPaintArgs& Args,
        const FGeometry& AllottedGeometry,
        const FSlateRect& MyCullingRect,
        FSlateWindowElementList& OutDrawElements,
        int32 LayerId,
        const FWidgetStyle& InWidgetStyle,
        bool bParentEnabled) const override;

    virtual FVector2D ComputeDesiredSize(float LayoutScaleMultiplier) const override;

private:

    /** Fetches latest quest states from QuestManager and triggers repaint */
    void RefreshData();

    // One entry per quest line to draw
    struct FQuestDisplayEntry
    {
        FString Name;
        EQuestState State; // included so we can color per state
        FString Line;      // pre-built display string e.g. "[ACTIVE]  Find the cat"
    };

    TWeakObjectPtr<UGameInstance> GameInstanceRef;
    TArray<FQuestDisplayEntry> QuestEntries;
    FSlateFontInfo FontInfo;
    FTimerHandle RefreshTimerHandle;

    float CachedWidth = 200.f;
    float CachedHeight = 50.f;
};