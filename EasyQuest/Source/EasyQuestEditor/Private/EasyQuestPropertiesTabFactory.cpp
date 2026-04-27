#include "EasyQuestPropertiesTabFactory.h"
#include "EasyQuestEditorApp.h"
#include "EasyQuestAsset.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "EasyQuestNodeInfo.h"

EasyQuestPropertiesTabFactory::EasyQuestPropertiesTabFactory(TSharedPtr<class EasyQuestEditorApp> app) : FWorkflowTabFactory(FName("EasyQuestPropertiesTab"), app)
{
    _app = app;
    TabLabel = FText::FromString(TEXT("Properties"));

    ViewMenuDescription = FText::FromString(TEXT("Displays a properties view for the current asset."));
    ViewMenuTooltip = FText::FromString(TEXT("Show the properties view."));
}

TSharedRef<SWidget> EasyQuestPropertiesTabFactory::CreateTabBody(const FWorkflowTabSpawnInfo& Info) const
{
    TSharedPtr<EasyQuestEditorApp> app = _app.Pin();
    FPropertyEditorModule& propertyEditorModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>(TEXT("PropertyEditor"));

    FDetailsViewArgs detailsViewArgs;
    {
        detailsViewArgs.bAllowSearch = false;
        detailsViewArgs.bHideSelectionTip = true;
        detailsViewArgs.bLockable = false;
        detailsViewArgs.bSearchInitialKeyFocus = true;
        detailsViewArgs.bUpdatesFromSelection = false;
        detailsViewArgs.NotifyHook = nullptr;
        detailsViewArgs.bShowOptions = true;
        detailsViewArgs.bShowModifiedPropertiesOption = false;
        detailsViewArgs.bShowScrollBar = false;
    }

    TSharedPtr<IDetailsView> detailsView = propertyEditorModule.CreateDetailView(detailsViewArgs);
    detailsView->SetObject(app->GetWorkingAsset());

    TSharedPtr<IDetailsView> selectedNodeDetailsView = propertyEditorModule.CreateDetailView(detailsViewArgs);
    selectedNodeDetailsView->SetObject(nullptr);
    app->SetSelectedNodeDetailView(selectedNodeDetailsView);

    return SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        .HAlign(HAlign_Fill)
        [
            detailsView.ToSharedRef()
        ]
        +SVerticalBox::Slot()
        .FillHeight(1.0f)
        .HAlign(HAlign_Fill)
        [
            selectedNodeDetailsView.ToSharedRef()
        ];
}

FText EasyQuestPropertiesTabFactory::GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const
{
    return FText::FromString(TEXT("A properties view for the current asset"));
}
