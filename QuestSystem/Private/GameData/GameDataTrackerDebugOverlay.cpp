#include "GameData/GameDataTrackerDebugOverlay.h"
#include "GameData/GameDataTrackerSubsystem.h"
#include "GameData/GameDataTrackerSettings.h"
#include "GameData/GameDataTrackerTypes.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Fonts/SlateFontInfo.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"

// Layout constants
namespace DebugOverlayStyle
{
    static const float PaddingLeft = 8.f;
    static const float PaddingRight = 48.f;
    static const float PaddingTop = 8.f;
    static const float LineHeight = 18.f;
    static const float HeaderBottomGap = 6.f;  // extra space between header and first data line
}

void SGameDataTrackerDebugOverlay::Construct(const FArguments& InArgs)
{
    FontInfo = FCoreStyle::GetDefaultFontStyle("Mono", 10);
}

SGameDataTrackerDebugOverlay::~SGameDataTrackerDebugOverlay()
{
    Stop();
}

// Start / Stop — called by subsystem only

void SGameDataTrackerDebugOverlay::Start(UGameInstance* InGameInstance)
{
    if (!InGameInstance) return;

    GameInstanceRef = InGameInstance;

    UGameViewportClient* Viewport = InGameInstance->GetGameViewportClient();
    if (!Viewport) return;

    Viewport->AddViewportWidgetContent(
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .HAlign(HAlign_Right)
        .VAlign(VAlign_Top)
        [
            AsShared()
        ],
        100
    );

    const UGameDataTrackerSettings* Settings = GetDefault<UGameDataTrackerSettings>();
    const float Interval = Settings ? Settings->DebugOverlayUpdateInterval : 0.5f;

    InGameInstance->GetTimerManager().SetTimer(
        RefreshTimerHandle,
        FTimerDelegate::CreateSP(this, &SGameDataTrackerDebugOverlay::RefreshData),
        Interval,
        true,
        0.f // fire immediately on start
    );
}

void SGameDataTrackerDebugOverlay::Stop()
{
    if (GameInstanceRef.IsValid())
    {
        GameInstanceRef->GetTimerManager().ClearTimer(RefreshTimerHandle);

        if (UGameViewportClient* Viewport = GameInstanceRef->GetGameViewportClient())
        {
            Viewport->RemoveAllViewportWidgets();
        }
    }

    GameInstanceRef.Reset();
    DataLines.Empty();
}

// -----------------------------------------------------------------------------
// Data fetching — owned entirely by this widget
// -----------------------------------------------------------------------------

void SGameDataTrackerDebugOverlay::RefreshData()
{
    if (!GameInstanceRef.IsValid()) return;

    UGameDataTrackerSubsystem* Tracker =
        GameInstanceRef->GetSubsystem<UGameDataTrackerSubsystem>();
    if (!Tracker) return;

    DataLines.Empty();

    TMap<FName, FGameDataFieldValue> AllData = Tracker->GetAllData();
    for (const auto& Pair : AllData)
    {
        FString ValueStr;
        switch (Pair.Value.DataType)
        {
        case ETrackedDataType::Integer: ValueStr = FString::FromInt(Pair.Value.IntValue);                break;
        case ETrackedDataType::Float:   ValueStr = FString::SanitizeFloat(Pair.Value.FloatValue);        break;
        case ETrackedDataType::Bool:    ValueStr = Pair.Value.BoolValue ? TEXT("true") : TEXT("false");  break;
        case ETrackedDataType::String:  ValueStr = Pair.Value.StringValue;                               break;
        case ETrackedDataType::CatRef:
            ValueStr = Pair.Value.LastCaughtCatRef.IsValid()
                ? Pair.Value.LastCaughtCatRef->GetName()
                : TEXT("Nullptr");
            break;
        case ETrackedDataType::Vector:
            ValueStr = FString::Printf(TEXT("(%.0f, %.0f, %.0f)"),
                Pair.Value.VectorValue.X,
                Pair.Value.VectorValue.Y,
                Pair.Value.VectorValue.Z);
            break;
        }
        DataLines.Add(FString::Printf(TEXT("[%s]  %s"), *Pair.Key.ToString(), *ValueStr));
    }

    // Measure actual text width using Slate font measure service
    // Reset to 0 every refresh so size shrinks/grows correctly
    float MaxWidth = 0.f;

    const TSharedRef<FSlateFontMeasure> FontMeasureService =
        FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

    FVector2D HeaderSize = FontMeasureService->Measure(TEXT("=== GameDataTracker ==="), FontInfo);
    MaxWidth = FMath::Max(MaxWidth, (float)HeaderSize.X);

    for (const FString& Line : DataLines)
    {
        FVector2D LineSize = FontMeasureService->Measure(Line, FontInfo);
        MaxWidth = FMath::Max(MaxWidth, (float)LineSize.X);
    }

    CachedWidth = MaxWidth + DebugOverlayStyle::PaddingLeft + DebugOverlayStyle::PaddingRight;
    CachedHeight = DebugOverlayStyle::PaddingTop
        + DebugOverlayStyle::LineHeight           // header line
        + DebugOverlayStyle::HeaderBottomGap
        + FMath::Max(1, DataLines.Num()) * DebugOverlayStyle::LineHeight;

    Invalidate(EInvalidateWidget::LayoutAndVolatility);
}

// -----------------------------------------------------------------------------
// Size & Drawing — owned entirely by this widget
// -----------------------------------------------------------------------------

FVector2D SGameDataTrackerDebugOverlay::ComputeDesiredSize(float) const
{
    return FVector2D(CachedWidth, CachedHeight);
}

int32 SGameDataTrackerDebugOverlay::OnPaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled) const
{
    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();

    // Semi-transparent background
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId,
        AllottedGeometry.ToPaintGeometry(
            FVector2f(LocalSize.X, LocalSize.Y),
            FSlateLayoutTransform()
        ),
        FCoreStyle::Get().GetBrush("WhiteBrush"),
        ESlateDrawEffect::None,
        FLinearColor(0.f, 0.f, 0.f, 0.6f)
    );

    // Header
    FSlateDrawElement::MakeText(
        OutDrawElements,
        LayerId + 1,
        AllottedGeometry.ToPaintGeometry(
            FVector2f(LocalSize.X, LocalSize.Y),
            FSlateLayoutTransform(FVector2f(DebugOverlayStyle::PaddingLeft, DebugOverlayStyle::PaddingTop))
        ),
        TEXT("=== GameDataTracker ==="),
        FontInfo,
        ESlateDrawEffect::None,
        FLinearColor(0.2f, 1.f, 0.4f, 1.f)
    );

    // First data row starts below header + gap
    float Y = DebugOverlayStyle::PaddingTop
        + DebugOverlayStyle::LineHeight
        + DebugOverlayStyle::HeaderBottomGap;

    if (DataLines.IsEmpty())
    {
        FSlateDrawElement::MakeText(
            OutDrawElements,
            LayerId + 1,
            AllottedGeometry.ToPaintGeometry(
                FVector2f(LocalSize.X, LocalSize.Y),
                FSlateLayoutTransform(FVector2f(DebugOverlayStyle::PaddingLeft, Y))
            ),
            TEXT("No data recorded yet."),
            FontInfo,
            ESlateDrawEffect::None,
            FLinearColor(0.5f, 0.5f, 0.5f, 1.f)
        );
    }
    else
    {
        for (const FString& Line : DataLines)
        {
            FSlateDrawElement::MakeText(
                OutDrawElements,
                LayerId + 1,
                AllottedGeometry.ToPaintGeometry(
                    FVector2f(LocalSize.X, LocalSize.Y),
                    FSlateLayoutTransform(FVector2f(DebugOverlayStyle::PaddingLeft, Y))
                ),
                Line,
                FontInfo,
                ESlateDrawEffect::None,
                FLinearColor::White
            );
            Y += DebugOverlayStyle::LineHeight;
        }
    }

    return LayerId + 1;
}