#include "ActorComponent.h"

#include "GameFramework/Actor.h"


UActorComponent* UActorComponent::Duplicate()
{
    // 🌟 현재 객체의 타입과 동일한 새 객체 생성
    UActorComponent* NewComponent = FObjectFactory::ConstructObject<UActorComponent>();

    // 🌟 속성 복사
    NewComponent->CopyPropertiesFrom(this);

    return NewComponent;
}


void UActorComponent::CopyPropertiesFrom(UObject* InSourceComponent)
{
    UActorComponent* SourceComponent = Cast<UActorComponent>(InSourceComponent);
    if (!SourceComponent)
        return;

    // 🌟 논리적 상태 복사
    this->bIsActive = SourceComponent->bIsActive;
    this->bHasBeenInitialized = SourceComponent->bHasBeenInitialized;
    this->bHasBegunPlay = SourceComponent->bHasBegunPlay;
    this->bIsBeingDestroyed = false;  // 새 복제본은 삭제 상태 아님

    // 🌟 컴포넌트의 고유 데이터 복사
    this->bAutoActive = SourceComponent->bAutoActive;

    // ❌ Owner는 복사하지 않음 (새로운 Actor가 소유해야 함)
    this->Owner = nullptr;
}

void UActorComponent::InitializeComponent()
{
    assert(!bHasBeenInitialized);

    bHasBeenInitialized = true;
}

void UActorComponent::UninitializeComponent()
{
    assert(bHasBeenInitialized);

    bHasBeenInitialized = false;
}

void UActorComponent::BeginPlay()
{
    bHasBegunPlay = true;
}

void UActorComponent::TickComponent(float DeltaTime)
{
}

void UActorComponent::OnComponentDestroyed()
{
}

void UActorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    assert(bHasBegunPlay);

    bHasBegunPlay = false;
}

void UActorComponent::DestroyComponent()
{
    if (bIsBeingDestroyed)
    {
        return;
    }

    bIsBeingDestroyed = true;

    // Owner에서 Component 제거하기
    if (AActor* MyOwner = GetOwner())
    {
        MyOwner->RemoveOwnedComponent(this);
        if (MyOwner->GetRootComponent() == this)
        {
            MyOwner->SetRootComponent(nullptr);
        }
    }

    if (bHasBegunPlay)
    {
        EndPlay(EEndPlayReason::Destroyed);
    }

    if (bHasBeenInitialized)
    {
        UninitializeComponent();
    }

    OnComponentDestroyed();

    // 나중에 ProcessPendingDestroyObjects에서 실제로 제거됨
    GUObjectArray.MarkRemoveObject(this);
}

void UActorComponent::Activate()
{
    // TODO: Tick 다시 재생
    bIsActive = true;
}

void UActorComponent::Deactivate()
{
    // TODO: Tick 멈추기
    bIsActive = false;
}
