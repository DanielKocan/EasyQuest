#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

/**
 * DEBUG OVERLAY
 *
 * A self-contained Slate widget that:
 *  - Adds itself to the viewport
 *  - Owns its own refresh timer
 *  - Fetches data directly from UGameDataTrackerSubsystem
 *  - Draws everything (background + text)
 *  - Removes itself from the viewport on Stop()
 *
 * The subsystem only creates it and calls Start/Stop.
 * All logic lives here.
 */
class SGameDataTrackerDebugOverlay : public SLeafWidget
{
public:
    SLATE_BEGIN_ARGS(SGameDataTrackerDebugOverlay) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);
    virtual ~SGameDataTrackerDebugOverlay();

    /** Called by subsystem — adds to viewport and starts refresh timer */
    void Start(UGameInstance* InGameInstance);

    /** Called by subsystem — removes from viewport and clears timer */
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

    /** Fetches latest data from subsystem and triggers repaint */
    void RefreshData();

    TWeakObjectPtr<UGameInstance> GameInstanceRef;
    TArray<FString> DataLines;
    FSlateFontInfo FontInfo;
    FTimerHandle RefreshTimerHandle;

    // Cached size computed in RefreshData using actual font measurement
    float CachedWidth = 200.f;
    float CachedHeight = 50.f;
};