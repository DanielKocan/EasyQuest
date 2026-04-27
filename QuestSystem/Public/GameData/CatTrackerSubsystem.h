#pragma once
#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CatTrackerSubsystem.generated.h"

class ACat;

UCLASS()
class WHISKEREDAWAY_API UCatTrackerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Cat Tracker")
    void RegisterCaughtCat(ACat* Cat);

    UFUNCTION(BlueprintCallable, Category = "Cat Tracker")
    void UnregisterCaughtCat(ACat* Cat);

    UFUNCTION(BlueprintCallable, Category = "Cat Tracker")
    void ClearAllCaughtCats();

    UFUNCTION(BlueprintCallable, Category = "Cat Tracker")
    void DestroyAllCaughtCats();

    UFUNCTION(BlueprintPure, Category = "Cat Tracker")
    TArray<ACat*> GetCaughtCats() const { return CaughtCats; }

    UFUNCTION(BlueprintPure, Category = "Cat Tracker")
    int32 GetCaughtCatCount() const { return CaughtCats.Num(); }

private:
    UPROPERTY()
    TArray<ACat*> CaughtCats;
};