#pragma once

#include "CoreMinimal.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

/**
You need a tabFacotry for each tab that you want to show up in the aplication
Basically: Creates the actual tab content
**/
class EasyQuestPropertiesTabFactory : public FWorkflowTabFactory
{
public:
	EasyQuestPropertiesTabFactory(TSharedPtr<class EasyQuestEditorApp> app);

	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override;
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override;

protected:
	TWeakPtr<class EasyQuestEditorApp> _app;
};
