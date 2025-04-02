#pragma once

#include "Runtime/CoreUObject/UObject/Object.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"

// 헤더만 만들어 둠, 현재 기능 업음
class ULevel : public UObject
{
    DECLARE_CLASS(ULevel, UObject)

public:
    ULevel() = default;
    virtual ~ULevel();

    FString LevelName;
    UWorld* OwningWorld;

    void Initialize();
    void BeginPlay();
    void Tick(float DeltaTime);
    void EndPlay();
    void Release();

    template <typename T>
        requires std::derived_from<T, AActor>
    T* SpawnActor();
    bool DestroyActor(AActor* ThisActor);

    const TSet<AActor*>& GetActors() const { return ActorsArray; }

    void AddActor(AActor* InActor);

    void CreateLevelObjects();
    void ReleaseLevelObjects();

    // Get level states
    bool IsLevelLoaded() const { return bIsLevelLoaded; }
    bool IsLevelLoading() const { return bIsLevelLoading; }

    // Level visibility
    void SetVisible(bool bInIsVisible);
    bool IsVisible() const { return bIsVisible; }

private:
    // Actors memeber
    TSet<AActor*> ActorsArray;
    TArray<AActor*> PendingBeginPlayActors;

    // Flags for level state
    uint8 bIsVisible : 1;
    uint8 bIsLevelLoaded : 1;
    uint8 bIsLevelLoading : 1;
};

template <typename T>
    requires std::derived_from<T, AActor>
T* ULevel::SpawnActor()
{
    T* Actor = FObjectFactory::ConstructObject<T>();

    Actor->SetWorld(OwningWorld);

    // Add actor to level
    ActorsArray.Add(Actor);
    PendingBeginPlayActors.Add(Actor);

    return Actor;
}