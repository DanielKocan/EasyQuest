#include "EasyQuestAppMode.h"
#include "EasyQuestEditorApp.h"
#include "EasyQuestPrimaryTabFactory.h"
#include "EasyQuestPropertiesTabFactory.h"

EasyQuestAppMode::EasyQuestAppMode(TSharedPtr<class EasyQuestEditorApp> app) : FApplicationMode(TEXT("EasyQuestAppMode"))
{
	_app = app;
	_tabs.RegisterFactory(MakeShareable(new EasyQuestPrimaryTabFactory(app)));
	_tabs.RegisterFactory(MakeShareable(new EasyQuestPropertiesTabFactory(app)));

	TabLayout = FTabManager::NewLayout("EasyQuestAppMode_Layout_v2")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.75)
					->AddTab(FName(TEXT("EasyQuestPrimaryTab")), ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.25)
					->AddTab(FName(TEXT("EasyQuestPropertiesTab")), ETabState::OpenedTab)
					->SetHideTabWell(true)
				)
			)
		);
}

void EasyQuestAppMode::RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager)
{
	TSharedPtr<EasyQuestEditorApp> app = _app.Pin();
	app->PushTabFactories(_tabs);
	FApplicationMode::RegisterTabFactories(InTabManager);
}

void EasyQuestAppMode::PreDeactivateMode()
{
	FApplicationMode::PreDeactivateMode();
}

void EasyQuestAppMode::PostActivateMode()
{
	FApplicationMode::PostActivateMode();

	TSharedPtr<EasyQuestEditorApp> app = _app.Pin();
	if (app.IsValid())
	{
		// Force both tabs open every time the mode activates
		app->GetTabManager()->TryInvokeTab(FName(TEXT("EasyQuestPrimaryTab")));
		app->GetTabManager()->TryInvokeTab(FName(TEXT("EasyQuestPropertiesTab")));
	}
}
