#include "EasyQuestAsset.h"
#include "UObject/ObjectSaveContext.h"

void UEasyQuestAsset::PreSave(FObjectPreSaveContext saveContext)
{
    Super::PreSave(saveContext);


    if (_onPreSaveListener) {
        _onPreSaveListener();
    }
}
