#include "GameData/CatTrackerSubsystem.h"
#include "Gameplay/CatSpawnerComponent.h"
#include "Developer/ChadCatJoey/Cat.h"

void UCatTrackerSubsystem::RegisterCaughtCat(ACat* Cat)
{
    if (Cat && !CaughtCats.Contains(Cat))
    {
        CaughtCats.Add(Cat);
        UE_LOG(LogTemp, Log, TEXT("CatTracker: Registered caught cat '%s', total: %d"),
            *Cat->GetName(), CaughtCats.Num());
    }
}

void UCatTrackerSubsystem::UnregisterCaughtCat(ACat* Cat)
{
    if (CaughtCats.Remove(Cat) > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("CatTracker: Unregistered cat '%s', remaining: %d"),
            *Cat->GetName(), CaughtCats.Num());
    }
}

void UCatTrackerSubsystem::ClearAllCaughtCats()
{
    CaughtCats.Empty();
    UE_LOG(LogTemp, Log, TEXT("CatTracker: Cleared all caught cats."));
}

void UCatTrackerSubsystem::DestroyAllCaughtCats()
{
    TArray<ACat*> CatsToDestroy = CaughtCats;
    CaughtCats.Empty();
    for (ACat* Cat : CatsToDestroy)
    {
        if (!IsValid(Cat)) { continue; }

        // Notify every spawner component: each one checks internally if it owns this cat and ignores it if not
        for (TObjectIterator<UCatSpawnerComponent> It; It; ++It)
        {
            if (It->GetWorld() == GetWorld())
            {
                It->OnCatReturnedToStore(Cat);
            }
        }

        Cat->Destroy();
    }
}