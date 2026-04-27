#include "EasyQuestEditorApp.h"
#include "EasyQuestAppMode.h"
#include "EasyQuestAsset.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "EasyQuestGraphSchema.h"
#include "EasyQuestGraphNode.h"
#include "EasyQuestEndGraphNode.h"
#include "EasyQuestStartGraphNode.h"
#include "EasyQuestNodeInfo.h"

DEFINE_LOG_CATEGORY_STATIC(EasyQuestEditorAppSub, Log, All);

void EasyQuestEditorApp::RegisterTabSpawners(const TSharedRef<class FTabManager>& tabManager) {
    FWorkflowCentricApplication::RegisterTabSpawners(tabManager);
}

void EasyQuestEditorApp::InitEditor(const EToolkitMode::Type mode, const TSharedPtr<class IToolkitHost>& initToolkitHost, UObject* inObject) {
    TArray<UObject*> objectsToEdit;
    objectsToEdit.Add(inObject);

    _workingAsset = Cast<UEasyQuestAsset>(inObject);

    _workingAsset->SetPreSaveListener([this]() { OnWorkingAssetPreSave(); });
    
    _workingGraph = FBlueprintEditorUtils::CreateNewGraph
    (
        _workingAsset,
        NAME_None,
        UEdGraph::StaticClass(),
        UEasyQuestGraphSchema::StaticClass()
    );

    InitAssetEditor(
        mode,
        initToolkitHost,
        TEXT("EasyQuestEditor"),
        FTabManager::FLayout::NullLayout,
        true, // createDefaultStandaloneMenu 
        true,  // createDefaultToolbar
        objectsToEdit);

    // Add our modes (just one for this example)
    AddApplicationMode(TEXT("EasyQuestAppMode"), MakeShareable(new EasyQuestAppMode(SharedThis(this))));

    // Set the mode
    SetCurrentMode(TEXT("EasyQuestAppMode"));

    UpdateEditorGraphFromWorkingAsset();
}

void EasyQuestEditorApp::SetSelectedNodeDetailView(TSharedPtr<class IDetailsView> detailsView)
{
    _selectedNodeDetailView = detailsView;
    //whenerever properties will be change it will call this function under it: OnNodeDetailViewPropertiesUpdated
    _selectedNodeDetailView->OnFinishedChangingProperties().AddRaw(this, &EasyQuestEditorApp::OnNodeDetailViewPropertiesUpdated);
}

void EasyQuestEditorApp::OnClose()
{
    UpdateWorkingAssetFromGraph();
    _workingAsset->SetPreSaveListener(nullptr);
    FAssetEditorToolkit::OnClose();
}

void EasyQuestEditorApp::OnNodeDetailViewPropertiesUpdated(const FPropertyChangedEvent& event)
{
    if (_workingGraphUi != nullptr) {
        //Get the node being modified
        UEasyQuestGraphNodeBase* easyQuestNode = GetSelectedNode(_workingGraphUi->GetSelectedNodes());
        if (easyQuestNode != nullptr){
            easyQuestNode->OnPropertiesChanged();
        }
        _workingGraphUi->NotifyGraphChanged();
    }
}

void EasyQuestEditorApp::OnWorkingAssetPreSave()
{
    // Update our asset from the graph jsut before saving it
    UpdateWorkingAssetFromGraph();
}

void EasyQuestEditorApp::UpdateWorkingAssetFromGraph()
{
    if (_workingAsset == nullptr || _workingGraph == nullptr) return;

    // Update the state we need into our saveable format
    UEasyQuestRuntimeGraph* runtimeGraph = NewObject<UEasyQuestRuntimeGraph>(_workingAsset);
    _workingAsset->Graph = runtimeGraph;

    TArray<std::pair<FGuid, FGuid>> connections;
    TMap<FGuid, UEasyQuestRuntimePin*> idToPinMap;

    // First create all the nodes/pins and record the connections
    for (UEdGraphNode* uiNode : _workingGraph->Nodes) {
        UEasyQuestRuntimeNode* runtimeNode = NewObject<UEasyQuestRuntimeNode>(runtimeGraph);
        runtimeNode->Position = FVector2D(uiNode->NodePosX, uiNode->NodePosY);

        // Capture all pins
        for (UEdGraphPin* uiPin : uiNode->Pins) {
            UEasyQuestRuntimePin* runtimePin = NewObject<UEasyQuestRuntimePin>(runtimeNode);
            runtimePin->PinName = uiPin->PinName;
            runtimePin->PinId = uiPin->PinId;
            runtimePin->Parent = runtimeNode;

            // Only record the the output side of the connection since this is a directed graph
            if (uiPin->HasAnyConnections() && uiPin->Direction == EEdGraphPinDirection::EGPD_Output) {
                // Only 1 connection is allowed so just take the first one
                std::pair<FGuid, FGuid> connection = std::make_pair(uiPin->PinId, uiPin->LinkedTo[0]->PinId);
                connections.Add(connection);
            }

            idToPinMap.Add(uiPin->PinId, runtimePin);
            if (uiPin->Direction == EEdGraphPinDirection::EGPD_Input) {
                runtimeNode->InputPin = runtimePin;
            }
            else {
                runtimeNode->OutputPins.Add(runtimePin);
            }
        }

        UEasyQuestGraphNodeBase* uiDialogNode = Cast<UEasyQuestGraphNodeBase>(uiNode);
        runtimeNode->NodeInfo = DuplicateObject(uiDialogNode->GetNodeInfo(), runtimeNode); // Reason why we are doing this: to share exact same data class between runtime and editor data structures to minimazi copy pasting (we dont have rewrite diffrent classes)
        runtimeNode->NodeType = uiDialogNode->GetEasyQuestNodeType();

        runtimeGraph->Nodes.Add(runtimeNode);
    }

    // Now make all the connections
    for (std::pair<FGuid, FGuid> connection : connections) {
        UEasyQuestRuntimePin* pin1 = idToPinMap[connection.first];
        UEasyQuestRuntimePin* pin2 = idToPinMap[connection.second];
        pin1->Connections.Add(pin2); // output pin can still only have 1, but this is uniform now
    }
}

// This gets called when we are oppening the graph based on save date
void EasyQuestEditorApp::UpdateEditorGraphFromWorkingAsset()
{
    if (_workingAsset->Graph == nullptr) {
        _workingGraph->GetSchema()->CreateDefaultNodesForGraph(*_workingGraph);
        return;
    }

    // Create all the nodes/pins first
    TArray<std::pair<FGuid, FGuid>> connections;
    TMap<FGuid, UEdGraphPin*> idToPinMap;

    for (UEasyQuestRuntimeNode* runtimeNode : _workingAsset->Graph->Nodes) {
        UEasyQuestGraphNodeBase* newNode = nullptr;
        if (runtimeNode->NodeType == EEasyQuestNodeType::StartNode)
        {
            newNode = NewObject<UEasyQuestStartGraphNode>(_workingGraph);
        }
        else if (runtimeNode->NodeType == EEasyQuestNodeType::DialogNode)
        {
            newNode = NewObject<UEasyQuestGraphNode>(_workingGraph);
        }
        else if (runtimeNode->NodeType == EEasyQuestNodeType::EndNode)
        {
            newNode = NewObject<UEasyQuestEndGraphNode>(_workingGraph);
        }
        else
        {
            UE_LOG(EasyQuestEditorAppSub, Error, TEXT("EasyQuestEditorApp::UpdateEditorGraphFromWorkingAsset: Unknown node type"));
            continue;
        }
        newNode->CreateNewGuid();
        newNode->NodePosX = runtimeNode->Position.X;
        newNode->NodePosY = runtimeNode->Position.Y;

        if (runtimeNode->NodeInfo != nullptr)
        {
            newNode->SetNodeInfo(DuplicateObject(runtimeNode->NodeInfo, newNode));
        }
        else
        {
            newNode->InitNodeInfo(newNode);
        }

        if (runtimeNode->InputPin != nullptr) {
            UEasyQuestRuntimePin* pin = runtimeNode->InputPin;
            UEdGraphPin* uiPin = newNode->CreateEasyQuestPin(EEdGraphPinDirection::EGPD_Input, pin->PinName);
            uiPin->PinId = pin->PinId;

            for (UEasyQuestRuntimePin* connectedPin : pin->Connections) {
                connections.Add(std::make_pair(pin->PinId, connectedPin->PinId));
            }
            idToPinMap.Add(pin->PinId, uiPin);
        }

        for (UEasyQuestRuntimePin* pin : runtimeNode->OutputPins) {
            UEdGraphPin* uiPin = newNode->CreateEasyQuestPin(EEdGraphPinDirection::EGPD_Output, pin->PinName);
            uiPin->PinId = pin->PinId;

            for (UEasyQuestRuntimePin* connectedPin : pin->Connections) {
                connections.Add(std::make_pair(pin->PinId, connectedPin->PinId));
            }
            idToPinMap.Add(pin->PinId, uiPin);
        }

        _workingGraph->AddNode(newNode, true, true);
    }

    for (std::pair<FGuid, FGuid> connection : connections) {
        UEdGraphPin* fromPin = idToPinMap[connection.first];
        UEdGraphPin* toPin = idToPinMap[connection.second];
        fromPin->LinkedTo.Add(toPin);
        toPin->LinkedTo.Add(fromPin);
    }
}

// Return the first UEasyGraphNode in the given selection or nullptr if there is none
UEasyQuestGraphNodeBase* EasyQuestEditorApp::GetSelectedNode(const FGraphPanelSelectionSet& selection)
{
    for (UObject* obj : selection)
    {
        UEasyQuestGraphNodeBase* node = Cast<UEasyQuestGraphNodeBase>(obj);
        if (node != nullptr)
        {
            return node;
        }
    }

    return nullptr;
}


void EasyQuestEditorApp::OnGraphSelectionChanged(const FGraphPanelSelectionSet& selection)
{
    if (!_selectedNodeDetailView.IsValid()) return;

    UEasyQuestGraphNodeBase* selectedNode = GetSelectedNode(selection);
    if (selectedNode != nullptr)
    {
        _selectedNodeDetailView->SetObject(selectedNode->GetNodeInfo());
    }
    else
    {
        _selectedNodeDetailView->SetObject(nullptr);
    }
}