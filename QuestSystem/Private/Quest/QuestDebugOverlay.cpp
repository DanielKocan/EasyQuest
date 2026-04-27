#include "Quest/QuestDebugOverlay.h"
#include "Quest/QuestManager.h"
#include "Quest/BaseQuest.h"
#include "Quest/QuestSettings.h"
#include "Engine/GameInstance.h"
#include "Engine/GameViewportClient.h"
#include "Rendering/DrawElements.h"
#include "Styling/CoreStyle.h"
#include "Fonts/SlateFontInfo.h"
#include "Fonts/FontMeasure.h"
#include "Framework/Application/SlateApplication.h"

// -----------------------------------------------------------------------------
// Layout constants — tweak to adjust spacing
// -----------------------------------------------------------------------------
namespace QuestDebugOverlayStyle
{
    static const float PaddingLeft = 8.f;
    static const float PaddingRight = 48.f;
    static const float PaddingTop = 8.f;
    static const float LineHeight = 18.f;
    static const float HeaderBottomGap = 6.f;
    static const float GroupGap = 6.f;  // extra space between state groups
}

static FString QuestStateToString(EQuestState State)
{
    switch (State)
    {
    case EQuestState::Blocked:   return TEXT("[BLOCKED]  ");
    case EQuestState::Available: return TEXT("[READY]    ");
    case EQuestState::Active:    return TEXT("[ACTIVE]   ");
    case EQuestState::Complete:  return TEXT("[DONE]     ");
    default:                     return TEXT("[UNKNOWN]  ");
    }
}

static FLinearColor QuestStateToColor(EQuestState State)
{
    switch (State)
    {
    case EQuestState::Blocked:   return FLinearColor(0.5f, 0.5f, 0.5f, 1.f); // grey
    case EQuestState::Available: return FLinearColor(0.2f, 0.6f, 1.f, 1.f); // blue
    case EQuestState::Active:    return FLinearColor(0.2f, 1.f, 0.4f, 1.f); // green
    case EQuestState::Complete:  return FLinearColor(1.f, 0.8f, 0.2f, 1.f); // gold
    default:                     return FLinearColor::White;
    }
}

void SQuestDebugOverlay::Construct(const FArguments& InArgs)
{
    FontInfo = FCoreStyle::GetDefaultFontStyle("Mono", 10);
}

SQuestDebugOverlay::~SQuestDebugOverlay()
{
    Stop();
}

// Start / Stop — called by QuestManager only

void SQuestDebugOverlay::Start(UGameInstance* InGameInstance)
{
    if (!InGameInstance) return;

    GameInstanceRef = InGameInstance;

    UGameViewportClient* Viewport = InGameInstance->GetGameViewportClient();
    if (!Viewport) return;

    // Add to top-left corner (GameDataTracker is top-right)
    Viewport->AddViewportWidgetContent(
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .HAlign(HAlign_Left)
        .VAlign(VAlign_Top)
        [
            AsShared()
        ],
        100
    );

    const UQuestSettings* Settings = GetDefault<UQuestSettings>();
    const float Interval = 0.5f; // refresh every 0.5s

    InGameInstance->GetTimerManager().SetTimer(
        RefreshTimerHandle,
        FTimerDelegate::CreateSP(this, &SQuestDebugOverlay::RefreshData),
        Interval,
        true,
        0.f
    );
}

void SQuestDebugOverlay::Stop()
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
    QuestEntries.Empty();
}

// Data fetching — owned entirely by this widget

void SQuestDebugOverlay::RefreshData()
{
    if (!GameInstanceRef.IsValid()) return;

    UQuestManager* QuestManager =
        GameInstanceRef->GetSubsystem<UQuestManager>();
    if (!QuestManager) return;

    QuestEntries.Empty();

    // Sort into groups: Active first, then Available, Blocked, Complete
    const TArray<EQuestState> StateOrder =
    {
        EQuestState::Active,
        EQuestState::Available,
        EQuestState::Blocked,
        EQuestState::Complete
    };

    for (EQuestState GroupState : StateOrder)
    {
        bool bAddedAny = false;

        for (UBaseQuest* Quest : QuestManager->GetQuestPool())
        {
            if (!Quest || Quest->GetState() != GroupState) continue;

            FQuestDisplayEntry Entry;
            Entry.Name = Quest->GetQuestName();
            Entry.State = GroupState;
            // Add visibility indicator: [V] = visible; [H] = hidden;
            FString VisibilityTag = Quest->IsVisible() ? TEXT(" [V]") : TEXT(" [H]");
            Entry.Line = QuestStateToString(GroupState) + Quest->GetQuestName() + VisibilityTag;
            QuestEntries.Add(Entry);
            bAddedAny = true;
        }

        // Add blank separator after each group that had entries
        if (bAddedAny)
        {
            FQuestDisplayEntry Separator;
            Separator.Line = TEXT("");
            Separator.State = GroupState;
            QuestEntries.Add(Separator);
        }
    }

    // Measure width
    float MaxWidth = 0.f;

    const TSharedRef<FSlateFontMeasure> FontMeasureService = FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

    FVector2D HeaderSize = FontMeasureService->Measure(TEXT("=== Quest Debug ==="), FontInfo);
    MaxWidth = FMath::Max(MaxWidth, (float)HeaderSize.X);

    for (const FQuestDisplayEntry& Entry : QuestEntries)
    {
        if (Entry.Line.IsEmpty()) continue;
        FVector2D LineSize = FontMeasureService->Measure(Entry.Line, FontInfo);
        MaxWidth = FMath::Max(MaxWidth, (float)LineSize.X);
    }

    CachedWidth = MaxWidth + QuestDebugOverlayStyle::PaddingLeft + QuestDebugOverlayStyle::PaddingRight;
    CachedHeight = QuestDebugOverlayStyle::PaddingTop
        + QuestDebugOverlayStyle::LineHeight           // header
        + QuestDebugOverlayStyle::HeaderBottomGap
        + FMath::Max(1, QuestEntries.Num()) * QuestDebugOverlayStyle::LineHeight;

    Invalidate(EInvalidateWidget::LayoutAndVolatility);
}

// Size & Drawing — owned entirely by this widget

FVector2D SQuestDebugOverlay::ComputeDesiredSize(float) const
{
    return FVector2D(CachedWidth, CachedHeight);
}

int32 SQuestDebugOverlay::OnPaint(
    const FPaintArgs& Args,
    const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect,
    FSlateWindowElementList& OutDrawElements,
    int32 LayerId,
    const FWidgetStyle& InWidgetStyle,
    bool bParentEnabled) const
{
    const FVector2D LocalSize = AllottedGeometry.GetLocalSize();

    // Background
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
            FSlateLayoutTransform(FVector2f(QuestDebugOverlayStyle::PaddingLeft, QuestDebugOverlayStyle::PaddingTop))
        ),
        TEXT("=== Quest Debug ==="),
        FontInfo,
        ESlateDrawEffect::None,
        FLinearColor(1.f, 0.6f, 0.1f, 1.f) // orange header to distinguish from GameDataTracker
    );

    float Y = QuestDebugOverlayStyle::PaddingTop
        + QuestDebugOverlayStyle::LineHeight
        + QuestDebugOverlayStyle::HeaderBottomGap;

    if (QuestEntries.IsEmpty())
    {
        FSlateDrawElement::MakeText(
            OutDrawElements,
            LayerId + 1,
            AllottedGeometry.ToPaintGeometry(
                FVector2f(LocalSize.X, LocalSize.Y),
                FSlateLayoutTransform(FVector2f(QuestDebugOverlayStyle::PaddingLeft, Y))
            ),
            TEXT("No quests loaded."),
            FontInfo,
            ESlateDrawEffect::None,
            FLinearColor(0.5f, 0.5f, 0.5f, 1.f)
        );
    }
    else
    {
        for (const FQuestDisplayEntry& Entry : QuestEntries)
        {
            if (!Entry.Line.IsEmpty())
            {
                FSlateDrawElement::MakeText(
                    OutDrawElements,
                    LayerId + 1,
                    AllottedGeometry.ToPaintGeometry(
                        FVector2f(LocalSize.X, LocalSize.Y),
                        FSlateLayoutTransform(FVector2f(QuestDebugOverlayStyle::PaddingLeft, Y))
                    ),
                    Entry.Line,
                    FontInfo,
                    ESlateDrawEffect::None,
                    QuestStateToColor(Entry.State)
                );
            }
            Y += QuestDebugOverlayStyle::LineHeight;
        }
    }

    return LayerId + 1;
}