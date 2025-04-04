#pragma once
#include "GameFramework/Actor.h"
#include "Engine/Classes/Components/StaticMeshComponent.h"

class UStaticMeshComponent;
class AStaticMeshActor : public AActor
{
    DECLARE_CLASS(AStaticMeshActor, AActor)

public:
    AStaticMeshActor();

    AStaticMeshActor* Duplicate() override {
        AStaticMeshActor* NewActor = FObjectFactory::ConstructObject<AStaticMeshActor>();

        if (StaticMeshComponent) {
            // 컴포넌트를 액터에 추가 (내부적으로 OwnedComponents에 등록됨)
            UStaticMeshComponent* NewMeshComp = NewActor->AddComponent<UStaticMeshComponent>();
            NewMeshComp->CopyPropertiesFrom(StaticMeshComponent);
            NewActor->StaticMeshComponent = NewMeshComp;
            //NewActor->RootComponent = NewMeshComp;

            NewMeshComp->SetStaticMesh(StaticMeshComponent->GetStaticMesh());
        }
        NewActor->CopyPropertiesFrom(this);
        NewActor->DuplicateSubObjects(this);
        
        return NewActor;
    }

    UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }

private:
    UStaticMeshComponent* StaticMeshComponent = nullptr;
};
