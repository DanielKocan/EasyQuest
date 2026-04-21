# EasyQuest

A visual quest graph editor plugin for Unreal Engine 5, built to make quest authoring fast and intuitive directly inside the editor.

![Unreal Engine](https://img.shields.io/badge/Unreal%20Engine-5-blue)
![License](https://img.shields.io/badge/license-MIT-green)

---

[📖 Wiki/Documentation - manual and tutorials](https://github.com/DanielKocan/EasyQuest/wiki)

---

## Overview

EasyQuest provides a node-based graph editor for designing quest systems in Unreal Engine 5. Quests are authored visually as graphs, serialized into a custom asset (`UEasyQuestAsset`), and consumed at runtime by your game's quest manager.

The plugin is split into two modules:
- **EasyQuestEditor** — the graph editor UI, node widgets, schema, and asset factory
- **EasyQuestRuntime** — the runtime graph data, asset, and node info classes

---

## Features

- **Visual graph editor** — node-based quest authoring with drag-and-drop connections
- **Multi-input pin support** — connect multiple predecessor quests to a single node, allowing complex unlock conditions (e.g. complete Quest A and Quest B to unlock Quest C)
- **Custom node properties** — each quest node exposes a title, description, completion checkbox, and dialogue responses directly in the graph
- **Properties panel** — selected node details shown in a dedicated dockable panel inside the editor
- **Asset-based** — quests are stored as `UEasyQuestAsset` assets, loadable via Project Settings or directly in code
- **Runtime graph** — serialized into `UEasyQuestRuntimeGraph` / `UEasyQuestRuntimeNode` / `UEasyQuestRuntimePin` for use at runtime without editor dependencies

---

## Plugin Structure

```
EasyQuest/
├── Source/
│   ├── EasyQuestEditor/        # Editor module
│   │   ├── Private/
│   │   │   ├── EasyQuestEditorApp.cpp      # Main editor application
│   │   │   ├── EasyQuestGraphSchema.cpp    # Connection rules
│   │   │   ├── EasyQuestGraphNode.cpp      # UEdGraphNode subclass
│   │   │   ├── SEasyQuestGraphNode.cpp     # Slate node widget
│   │   │   └── ...
│   │   └── Public/
│   └── EasyQuestRuntime/       # Runtime module
│       ├── Private/
│       │   ├── EasyQuestAsset.cpp
│       │   ├── EasyQuestRuntimeGraph.cpp
│       │   └── EasyQuestNodeInfo.cpp
│       └── Public/
│           ├── EasyQuestAsset.h
│           ├── EasyQuestRuntimeGraph.h     # Pin/Node/Graph runtime types
│           └── EasyQuestNodeInfo.h         # Quest node data
```

---

## Getting Started

### Installation

1. Copy the `EasyQuest` folder into your project's `Plugins/` directory
2. Regenerate project files
3. Enable the plugin in **Edit -> Plugins -> EasyQuest**
4. Rebuild the project

### Creating a Quest Asset

1. In the Content Browser, right-click -> **Miscellaneous -> EasyQuest Asset**
2. Double-click the asset to open the graph editor
3. Add quest nodes, connect them, and fill in the node properties

### Referencing the Asset at Runtime

Set the asset path in **Project Settings -> Quest System -> Main Quest Graph Asset**, then load it from your quest manager:

```cpp
UEasyQuestAsset* Asset = Cast<UEasyQuestAsset>(Settings->MainQuestGraphAsset.TryLoad());
```

---

## Node Properties

Each quest node (`UEasyQuestNodeInfo`) exposes:

| Property | Type | Description |
|---|---|---|
| `Title` | `FText` | Display name of the quest |
| `QuestDescription` | `FText` | Full description shown to the player |
| `isCompleted` | `bool` | Completion state (editable in graph) |
| `DialogResponses` | `TArray<FText>` | Dialogue response options for this quest |

---

## Runtime Types

| Class | Description |
|---|---|
| `UEasyQuestAsset` | The main asset containing the runtime graph |
| `UEasyQuestRuntimeGraph` | Container for all runtime nodes |
| `UEasyQuestRuntimeNode` | Represents a single quest node at runtime |
| `UEasyQuestRuntimePin` | Represents a connection pin; supports multiple connections |

### Pin Connections

Output pins support a **single outgoing connection** (one quest leads to one successor). Input pins support **multiple incoming connections** (a quest can require multiple predecessors to be completed before unlocking).

```cpp
// Iterating over connections at runtime
for (UEasyQuestRuntimePin* ConnectedPin : OutputPin->Connections)
{
    UEasyQuestRuntimeNode* ChildNode = ConnectedPin->Parent;
    // ...
}
```

---

## Integration Example

```cpp
// Load quests from asset
void UQuestManager::LoadQuestsFromAsset(UEasyQuestAsset* QuestAsset)
{
    for (UEasyQuestRuntimeNode* Node : QuestAsset->Graph->Nodes)
    {
        UEasyQuestNodeInfo* NodeInfo = Cast<UEasyQuestNodeInfo>(Node->NodeInfo);
        if (NodeInfo)
        {
            UBaseQuest* Quest = CreateQuest(
                NodeInfo->Title.ToString(),
                NodeInfo->QuestDescription.ToString(),
                TArray<UBaseQuest*>()
            );
        }
    }

    // Set up dependencies from pin connections
    for (auto& Pair : NodeToQuestMap)
    {
        UEasyQuestRuntimeNode* Node = Pair.Key;
        UBaseQuest* Quest = Pair.Value;

        for (UEasyQuestRuntimePin* OutputPin : Node->OutputPins)
        {
            for (UEasyQuestRuntimePin* ConnectedPin : OutputPin->Connections)
            {
                UEasyQuestRuntimeNode* ChildNode = ConnectedPin->Parent;
                UBaseQuest* ChildQuest = NodeToQuestMap.FindRef(ChildNode);
                if (ChildQuest)
                {
                    ChildQuest->AddDependency(Quest);
                }
            }
        }
    }
}
```

---

## Requirements

- Unreal Engine 5.x
- C++ project

---

## License

MIT License - see [LICENSE](LICENSE) for details.