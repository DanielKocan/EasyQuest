#pragma once

#include "CoreMinimal.h"
#include "SGraphPanel.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

/** 
Aplication that owns the editor window 
Basically: The main editor window that opens when you double-click your asset
**/
class EasyQuestEditorApp : public FWorkflowCentricApplication, public FEditorUndoClient, public FNotifyHook
{
public: // FWorkflowCentricApplication interface
    virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
    void InitEditor(const EToolkitMode::Type mode, const TSharedPtr<class IToolkitHost>& initToolkitHost, UObject* inObject);

    class UEasyQuestAsset* GetWorkingAsset() { return _workingAsset; }
    class UEdGraph* GetWorkingGraph() { return _workingGraph; } //Data

    void SetWorkingGraphUi(TSharedPtr<SGraphEditor> workingGraphUi) { _workingGraphUi = workingGraphUi; } //UI
    void SetSelectedNodeDetailView(TSharedPtr<class IDetailsView> detailsView);
    void OnGraphSelectionChanged(const FGraphPanelSelectionSet& selection);

public: // FAssetEditorToolkit interface
    virtual FName GetToolkitFName() const override { return FName(TEXT("EasyQuestEditorApp")); }
    virtual FText GetBaseToolkitName() const override { return FText::FromString(TEXT("EasyQuestEditorApp")); }
    virtual FString GetWorldCentricTabPrefix() const override { return TEXT("EasyQuestEditorApp"); }
    virtual FLinearColor GetWorldCentricTabColorScale() const override { return FLinearColor(0.3f, 0.2f, 0.5f, 0.5f); }
    virtual FString GetDocumentationLink() const override { return TEXT(""); }
    virtual void OnToolkitHostingStarted(const TSharedRef<class IToolkit>& Toolkit) override {}
    virtual void OnToolkitHostingFinished(const TSharedRef<class IToolkit>& Toolkit) override {}

    virtual void OnClose() override;
    void OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& event);
    void OnWorkingAssetPreSave();

protected:
    // This will update working assets from the graph when we are ready to save
    void UpdateWorkingAssetFromGraph();
    // This will update the editor from the working asset when the editor comes up, and restores whaterver was there
    void UpdateEditorGraphFromWorkingAsset();
    class UEasyQuestGraphNodeBase* GetSelectedNode(const FGraphPanelSelectionSet& selection);

private:
    UPROPERTY()
    class UEasyQuestAsset* _workingAsset = nullptr;

    UPROPERTY()// Working graph model (NO UI, just data)
    class UEdGraph* _workingGraph = nullptr;

    // Working graph UI (the slate widget that displays the graph (_workingGraph))
    TSharedPtr<SGraphEditor> _workingGraphUi = nullptr;

    TSharedPtr<class IDetailsView> _selectedNodeDetailView = nullptr;
};