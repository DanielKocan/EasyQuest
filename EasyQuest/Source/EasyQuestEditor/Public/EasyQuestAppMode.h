#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/WorkflowTabManager.h"

/** 
Application mode for main behavior tree editing mode, You need one mode for each unique tab associated to that 
Basically: Defines the layout/mode of your editor (which tabs appear)
**/
class EasyQuestAppMode : public FApplicationMode
{
public:
	EasyQuestAppMode(TSharedPtr<class EasyQuestEditorApp> app);

	virtual void RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager) override;
	virtual void PreDeactivateMode() override;
	virtual void PostActivateMode() override;

protected:
	TWeakPtr<class EasyQuestEditorApp> _app;
	FWorkflowAllowedTabSet _tabs;
};