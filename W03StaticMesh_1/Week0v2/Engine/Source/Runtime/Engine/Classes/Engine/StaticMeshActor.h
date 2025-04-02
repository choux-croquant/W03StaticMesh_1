#pragma once
#include "GameFramework/Actor.h"

class UStaticMeshComponent;

class AStaticMeshActor : public AActor
{
    DECLARE_CLASS(AStaticMeshActor, AActor)

public:
    AStaticMeshActor();

    AStaticMeshActor* Duplicate() override {
        AStaticMeshActor* NewActor = FObjectFactory::ConstructObject<AStaticMeshActor>();
        NewActor->RootComponent = this->RootComponent;
        NewActor->CopyPropertiesFrom(this);
        NewActor->DuplicateSubObjects(this);
        
        return NewActor;
    }

    UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

private:
    UStaticMeshComponent* StaticMeshComponent = nullptr;
};
