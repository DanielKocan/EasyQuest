#include "EasyQuestPrimaryTabFactory.h"
#include "EasyQuestEditorApp.h"
#include "EasyQuestAsset.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "GraphEditor.h"
#include "Editor/UnrealEd/Public/Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"

EasyQuestPrimaryTabFactory::EasyQuestPrimaryTabFactory(TSharedPtr<class EasyQuestEditorApp> app) : FWorkflowTabFactory(FName("EasyQuestPrimaryTab"), app)
{
	_app = app;
	TabLabel = FText::FromString(TEXT("Primary"));

	ViewMenuDescription = FText::FromString(TEXT("Displays a primary view for whatever you want to do."));
	ViewMenuTooltip = FText::FromString(TEXT("Show the primary view."));
}

TSharedRef<SWidget> EasyQuestPrimaryTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<EasyQuestEditorApp> app = _app.Pin();

    SGraphEditor::FGraphEditorEvents graphEvents;
    graphEvents.OnSelectionChanged.BindRaw(app.Get(), &EasyQuestEditorApp::OnGraphSelectionChanged);

    TSharedPtr<SGraphEditor> graphEditor =
        SNew(SGraphEditor)
        .IsEditable(true)
        .GraphEvents(graphEvents)
        .GraphToEdit(app->GetWorkingGraph());

    app->SetWorkingGraphUi(graphEditor);

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .HAlign(HAlign_Fill)
        [
            graphEditor.ToSharedRef()
        ];
}

FText EasyQuestPrimaryTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
	return FText::FromString(TEXT("A primary view for doing primary things"));
}
