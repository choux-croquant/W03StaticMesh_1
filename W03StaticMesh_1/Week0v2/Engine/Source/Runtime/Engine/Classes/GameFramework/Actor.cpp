#include "Actor.h"

#include "World.h"

void AActor::BeginPlay()
{
    // TODO: 나중에 삭제를 Pending으로 하던가 해서 복사비용 줄이기
    const auto CopyComponents = OwnedComponents;
    for (UActorComponent* Comp : CopyComponents)
    {
        Comp->BeginPlay();
    }
}

void AActor::Tick(float DeltaTime)
{
    // TODO: 임시로 Actor에서 Tick 돌리기
    // TODO: 나중에 삭제를 Pending으로 하던가 해서 복사비용 줄이기
    const auto CopyComponents = OwnedComponents;
    for (UActorComponent* Comp : CopyComponents)
    {
        Comp->TickComponent(DeltaTime);
    }
}

void AActor::Destroyed()
{
    // Actor가 제거되었을 때 호출하는 EndPlay
    EndPlay(EEndPlayReason::Destroyed);
}

void AActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    // 본인이 소유하고 있는 모든 컴포넌트의 EndPlay 호출
    for (UActorComponent* Component : GetComponents())
    {
        if (Component->HasBegunPlay())
        {
            Component->EndPlay(EndPlayReason);
        }
    }
    UninitializeComponents();
}

AActor* AActor::Duplicate()
{
    // 🌟 같은 클래스 타입의 새로운 액터 생성
    AActor* NewActor = new AActor();

    NewActor->RootComponent = this->RootComponent;

    // 🌟 기본 속성 복사 (위치, 회전, 스케일, 이름 등)
    NewActor->CopyPropertiesFrom(this);

    // 🌟 컴포넌트 및 자식 액터 깊은 복사
    NewActor->DuplicateSubObjects(this);


    // 🌟 액터 파괴 플래그 초기화
    NewActor->bActorIsBeingDestroyed = false;

    return NewActor;
}

void AActor::DuplicateSubObjects(AActor* SourceActor)
{
    // 🔥 1. 컴포넌트 깊은 복사
    for (UActorComponent* component : SourceActor-> GetComponents())
    {
        if (!component)
            continue;

        // 새로운 컴포넌트 복제
        UActorComponent* newcomponent = component->Duplicate();
        if (newcomponent)
        {
            OwnedComponents.Add(newcomponent);
            newcomponent->Owner = this;
        }
    }
}

void AActor::CopyPropertiesFrom(AActor* SourceActor)
{
    if (!SourceActor)
        return;

    // 🌟 기본적인 속성 복사
    this->SetActorLocation(SourceActor->GetActorLocation());
    this->SetActorRotation(SourceActor->GetActorRotation());
    this->SetActorScale(SourceActor->GetActorScale());
    this->SetActorLabel(SourceActor->GetActorLabel());
   
    // 🌟 기타 필요한 속성 추가 복사 가능
}



bool AActor::Destroy()
{
    if (!IsActorBeingDestroyed())
    {
        if (UWorld* World = GetWorld())
        {
            World->DestroyActor(this);
            bActorIsBeingDestroyed = true;
        }
    }

    return IsActorBeingDestroyed();
}



void AActor::RemoveOwnedComponent(UActorComponent* Component)
{
    OwnedComponents.Remove(Component);
}

void AActor::InitializeComponents()
{
    TSet<UActorComponent*> Components = GetComponents();
    for (UActorComponent* ActorComp : Components)
    {
        if (ActorComp->bAutoActive && !ActorComp->IsActive())
        {
            ActorComp->Activate();
        }

        if (!ActorComp->HasBeenInitialized())
        {
            ActorComp->InitializeComponent();
        }
    }
}

void AActor::UninitializeComponents()
{
    TSet<UActorComponent*> Components = GetComponents();
    for (UActorComponent* ActorComp : Components)
    {
        if (ActorComp->HasBeenInitialized())
        {
            ActorComp->UninitializeComponent();
        }
    }
}

bool AActor::SetRootComponent(USceneComponent* NewRootComponent)
{
    if (NewRootComponent == nullptr || NewRootComponent->GetOwner() == this)
    {
        if (RootComponent != NewRootComponent)
        {
            USceneComponent* OldRootComponent = RootComponent;
            RootComponent = NewRootComponent;

            OldRootComponent->SetupAttachment(RootComponent);
        }
        return true;
    }
    return false;
}

bool AActor::SetActorLocation(const FVector& NewLocation)
{
    if (RootComponent)
    {
        RootComponent->SetLocation(NewLocation);
        return true;
    }
    return false;
}

bool AActor::SetActorRotation(const FVector& NewRotation)
{
    if (RootComponent)
    {
        RootComponent->SetRotation(NewRotation);
        return true;
    }
    return false;
}

bool AActor::SetActorScale(const FVector& NewScale)
{
    if (RootComponent)
    {
        RootComponent->SetScale(NewScale);
        return true;
    }
    return false;
}
