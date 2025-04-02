#pragma once

#include "Level.h"

#include "Actors/Player.h"
#include "BaseGizmos/TransformGizmo.h"
#include "Camera/CameraComponent.h"
#include "LevelEditor/SLevelEditor.h"
#include "Engine/FLoaderOBJ.h"
#include "Engine/StaticMeshActor.h"
#include "Components/SkySphereComponent.h"
#include "Classes/Components/StaticMeshComponent.h"

ULevel::~ULevel()
{
}

void ULevel::Initialize()
{
    bIsLevelLoading = true;

    CreateLevelObjects();

    // Initialize end
    bIsLevelLoaded = true;
    bIsLevelLoading = false;
    bIsVisible = true;
}

void ULevel::AddActor(AActor* InActor)
{
    ActorsArray.Add(InActor);
}

void ULevel::CreateLevelObjects()
{  
}

void ULevel::ReleaseLevelObjects()
{
}

void ULevel::Tick(float DeltaTime)
{
    for (AActor* Actor : PendingBeginPlayActors)
    {
        Actor->BeginPlay();
    }
    PendingBeginPlayActors.Empty();

    // Update Actors tick
    for (AActor* Actor : ActorsArray)
    {
        Actor->Tick(DeltaTime);
    }
}

void ULevel::Release()
{
    EndPlay();

    for (AActor* Actor : ActorsArray)
    {
        TSet<UActorComponent*> Components = Actor->GetComponents();
        for (UActorComponent* Component : Components)
        {
            GUObjectArray.MarkRemoveObject(Component);
        }
        GUObjectArray.MarkRemoveObject(Actor);
    }
    ActorsArray.Empty();

    ReleaseLevelObjects();
    GUObjectArray.ProcessPendingDestroyObjects();
    bIsLevelLoaded = false;
}

void ULevel::SetVisible(bool bInIsVisible)
{
    if (bIsVisible != bInIsVisible)
    {
        bIsVisible = bInIsVisible;

        // Set actor visibility
        for (AActor* Actor : ActorsArray)
        {
            // TODO : add actor visibility flags
        }
    }
}

void ULevel::BeginPlay()
{
    for (AActor* Actor : ActorsArray)
    {
        Actor->BeginPlay();
    }
}

void ULevel::EndPlay()
{
    for (AActor* Actor : ActorsArray)
    {
        Actor->EndPlay(EEndPlayReason::WorldTransition);
    }
}

bool ULevel::DestroyActor(AActor* ThisActor)
{
    if (ThisActor->GetWorld() == nullptr || ThisActor->GetWorld() != OwningWorld)
    {
        return false;
    }

    if (ThisActor->IsActorBeingDestroyed())
    {
        return true;
    }

    ThisActor->Destroyed();

    if (ThisActor->GetOwner())
    {
        ThisActor->SetOwner(nullptr);
    }

    TSet<UActorComponent*> Components = ThisActor->GetComponents();
    for (UActorComponent* Component : Components)
    {
        Component->DestroyComponent();
    }

    // Remove actor from level
    ActorsArray.Remove(ThisActor);

    GUObjectArray.MarkRemoveObject(ThisActor);
    return true;
}