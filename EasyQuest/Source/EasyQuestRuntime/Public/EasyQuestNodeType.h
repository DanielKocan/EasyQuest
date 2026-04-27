#pragma once

#include "EasyQuestNodeType.generated.h"

UENUM()
enum class EEasyQuestNodeType
{
    Unknown,
    StartNode,
    DialogNode,
    EndNode
};