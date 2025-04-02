#pragma once
#include "Define.h"
#include "Container/Set.h"
#include "UObject/ObjectFactory.h"
#include "UObject/ObjectMacros.h"
#include "Engine/EngineTypes.h"
#include "Engine/Level.h"

class FObjectFactory;
class AActor;
class UObject;
class UGizmoArrowComponent;
class UCameraComponent;
class AEditorPlayer;
class USceneComponent;
class UTransformGizmo;

struct FWorldContext {
    UWorld* World;
    EWorldType::Type WorldType;     // Context가 World의 Type을 소유


};

class UWorld final : public UObject
{
    DECLARE_CLASS(UWorld, UObject)

public:
    UWorld() = default;

    // Level that currently activated
    ULevel* PersistentLevel;
    EWorldType::Type WorldType;

    static UWorld* CreateWorld(
        const EWorldType::Type InWorldType
    );

    void Initialize();
    void Tick(float DeltaTime);
    void Release();

    void CreateEditorObjects();
    void ReleaseEditorObjects();

    // Level management methods
    ULevel* CreateNewLevel(const FString& LevelName = TEXT("Default"));
    bool LoadLevel(const FString& LevelPath);
    bool UnloadLevel(ULevel* Level);
    void SetCurrentLevel(ULevel* Level);

    // Move spawnactor logic to level class
    template <typename T>
        requires std::derived_from<T, AActor>
    T* SpawnActor()
    {
        return PersistentLevel ? PersistentLevel->SpawnActor<T>() : nullptr;
    }

    bool DestroyActor(AActor* ThisActor)
    {
        return PersistentLevel ? PersistentLevel->DestroyActor(ThisActor) : false;
    }

private:
    // Editor related members
    AActor* SelectedActor = nullptr;
    USceneComponent* pickingGizmo = nullptr;
    UCameraComponent* camera = nullptr;
    AEditorPlayer* EditorPlayer = nullptr;
    UObject* worldGizmo = nullptr;
    UTransformGizmo* LocalGizmo = nullptr;

    // Level related members
    TArray<ULevel*> Levels;
    const FString DefaultMapName = TEXT("Default");

public:
    AActor* GetSelectedActor() const { return SelectedActor; }
    void SetPickedActor(AActor* InActor) { SelectedActor = InActor; }

    UObject* GetWorldGizmo() const { return worldGizmo; }
    USceneComponent* GetPickingGizmo() const { return pickingGizmo; }
    void SetPickingGizmo(UObject* Object);

    UCameraComponent* GetCamera() const { return camera; }
    AEditorPlayer* GetEditorPlayer() const { return EditorPlayer; }
    UTransformGizmo* GetLocalGizmo() const { return LocalGizmo; }

    // Level related getters
    ULevel* GetCurrentLevel() const { return PersistentLevel; }
    const TArray<ULevel*>& GetLevels() const { return Levels; }
};