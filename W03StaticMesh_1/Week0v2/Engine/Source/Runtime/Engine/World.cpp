#include "Engine/Source/Runtime/Engine/World.h"

#include "Actors/Player.h"
#include "BaseGizmos/TransformGizmo.h"
#include "Camera/CameraComponent.h"
#include "LevelEditor/SLevelEditor.h"
#include "Engine/FLoaderOBJ.h"
#include "Classes/Components/StaticMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SkySphereComponent.h"


UWorld* UWorld::CreateWorld(
    const EWorldType::Type InWorldType
)
{
    // Create world instance
    UWorld* NewWorld = FObjectFactory::ConstructObject<UWorld>();

    NewWorld->WorldType = InWorldType;

    // Additional setting for each world type
    switch (InWorldType)
    {
    case EWorldType::Editor:
        // TODO : settig for editor type world
        break;
    case EWorldType::Game:
        // TODO : setting for game type world
        break;
    case EWorldType::PIE:
        // TODO : setting for PIC type world
        break;
    case EWorldType::EditorPreview:
        // TODO : setting for editor preview type world
        break;
    default:
        break;
    }

    NewWorld->Initialize(InWorldType);
    return NewWorld;
}

void UWorld::Initialize(EWorldType::Type InWorldType)
{
    if (PersistentLevel == nullptr)
    {
        PersistentLevel = CreateNewLevel(DefaultMapName);
    }

    // 레벨 초기화
    if (PersistentLevel)
    {
        PersistentLevel->Initialize();
    }

    switch (InWorldType)
    {
    case EWorldType::Editor:
        CreateEditorObjects(InWorldType);
        break;
    case EWorldType::Game:
        // TODO : setting for game type world
        break;
    case EWorldType::PIE:
        CreateEditorObjects(InWorldType);
        break;
    case EWorldType::EditorPreview:
        // TODO : setting for editor preview type world
        break;
    default:
        break;
    }
}

void UWorld::CreateEditorObjects(EWorldType::Type InWorldType)
{
    if (EditorPlayer == nullptr)
    {
        EditorPlayer = FObjectFactory::ConstructObject<AEditorPlayer>();
    }

    if (camera == nullptr)
    {
        camera = FObjectFactory::ConstructObject<UCameraComponent>();
        camera->SetLocation(FVector(8.0f, 8.0f, 8.f));
        camera->SetRotation(FVector(0.0f, 45.0f, -135.0f));
    }

    if (LocalGizmo == nullptr && InWorldType == EWorldType::Editor)
    {
        LocalGizmo = SpawnActor<UTransformGizmo>();
    }
    
    /*if (worldGizmo == nullptr && InWorldType == EWorldType::Editor)
    {
        worldGizmo = FObjectFactory::ConstructObject<UObject>();
    }*/
}

void UWorld::ReleaseEditorObjects()
{
    if (LocalGizmo)
    {
        delete LocalGizmo;
        LocalGizmo = nullptr;
    }

    /*if (worldGizmo)
    {
        delete worldGizmo;
        worldGizmo = nullptr;
    }*/

    if (camera)
    {
        delete camera;
        camera = nullptr;
    }

    if (EditorPlayer)
    {
        delete EditorPlayer;
        EditorPlayer = nullptr;
    }
}

void UWorld::Tick(float DeltaTime)
{
    // Tick for editor objects
    if (camera) camera->TickComponent(DeltaTime);
    if (EditorPlayer) EditorPlayer->Tick(DeltaTime);
    if (LocalGizmo) LocalGizmo->Tick(DeltaTime);

    if (PersistentLevel)
    {
        PersistentLevel->Tick(DeltaTime);
    }

    // Tick for not current levels
    /*for (ULevel* Level : Levels)
    {
        if (Level != PersistentLevel && Level)
        {
            Level->Tick(DeltaTime);
        }
    }*/
}

void UWorld::Release()
{
    if (PersistentLevel)
    {
        PersistentLevel->Release();
        delete PersistentLevel;
        PersistentLevel = nullptr;
    }

    for (ULevel* Level : Levels)
    {
        if (Level && Level != PersistentLevel)
        {
            Level->Release();
            delete Level;
        }
    }

    Levels.Empty();

    pickingGizmo = nullptr;
    ReleaseEditorObjects();

    // Destroy pending objects
    GUObjectArray.ProcessPendingDestroyObjects();
}

ULevel* UWorld::CreateNewLevel(const FString& LevelName)
{
    ULevel* NewLevel = FObjectFactory::ConstructObject<ULevel>();
    NewLevel->LevelName = LevelName;
    NewLevel->OwningWorld = this;  // OwningWorld 설정

    // 레벨 목록에 추가
    Levels.Add(NewLevel);

    return NewLevel;
}

bool UWorld::LoadLevel(const FString& LevelPath)
{
    // TODO : add load logic

    //LoadedLevel->OwningWorld = this;
    return true;
}

bool UWorld::UnloadLevel(ULevel* Level)
{
    if (!Level || Level == PersistentLevel)
    {
        return false;
    }

    // 레벨 언로드 로직
    Level->Release();
    Levels.Remove(Level);
    delete Level;

    return true;
}

void UWorld::SetCurrentLevel(ULevel* Level)
{
    if (Level && Levels.Contains(Level))
    {
        PersistentLevel = Level;
    }
}

void UWorld::SetPickingGizmo(UObject* Object)
{
    pickingGizmo = Cast<USceneComponent>(Object);
}