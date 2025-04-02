#include "Components/SceneComponent.h"
#include "World.h"
#include "Math/JungleMath.h"
#include "UObject/ObjectFactory.h"
#include "UTextUUID.h"
USceneComponent::USceneComponent() :RelativeLocation(FVector(0.f, 0.f, 0.f)), RelativeRotation(FVector(0.f, 0.f, 0.f)), RelativeScale3D(FVector(1.f, 1.f, 1.f))
{
}

USceneComponent::~USceneComponent()
{
	if (uuidText) delete uuidText;
}
void USceneComponent::InitializeComponent()
{
    Super::InitializeComponent();

}

void USceneComponent::TickComponent(float DeltaTime)
{
	Super::TickComponent(DeltaTime);
}


int USceneComponent::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    int nIntersections = 0;
    return nIntersections;
}

FVector USceneComponent::GetForwardVector()
{
	FVector Forward = FVector(1.f, 0.f, 0.0f);
	Forward = JungleMath::FVectorRotate(Forward, QuatRotation);
	return Forward;
}

FVector USceneComponent::GetRightVector()
{
	FVector Right = FVector(0.f, 1.f, 0.0f);
	Right = JungleMath::FVectorRotate(Right, QuatRotation);
	return Right;
}

FVector USceneComponent::GetUpVector()
{
	FVector Up = FVector(0.f, 0.f, 1.0f);
	Up = JungleMath::FVectorRotate(Up, QuatRotation);
	return Up;
}


void USceneComponent::AddLocation(FVector _added)
{
	RelativeLocation = RelativeLocation + _added;

}

void USceneComponent::AddRotation(FVector _added)
{
	RelativeRotation = RelativeRotation + _added;

}

void USceneComponent::AddScale(FVector _added)
{
	RelativeScale3D = RelativeScale3D + _added;

}

USceneComponent* USceneComponent::Duplicate()
{
    // 🌟 현재 객체의 타입과 동일한 새 객체 생성
    USceneComponent* NewComponent = FObjectFactory::ConstructObject<USceneComponent>();

    // 🌟 속성 복사
    NewComponent->CopyPropertiesFrom(this);

    return NewComponent;
}

void USceneComponent::CopyPropertiesFrom(UObject* InSourceComponent)
{
    USceneComponent* SourceComponent = Cast<USceneComponent>(InSourceComponent);
    if (!SourceComponent)
        return;

    // 🌟 논리적 상태 복사
    this->RelativeLocation = SourceComponent->RelativeLocation;
    this->RelativeRotation = SourceComponent->RelativeRotation;
    this->QuatRotation = SourceComponent->QuatRotation;
    this->RelativeScale3D = SourceComponent->RelativeScale3D;
    this->AttachParent = SourceComponent->AttachParent;
    this->AttachChildren = SourceComponent->AttachChildren;

    // 🌟 컴포넌트의 고유 데이터 복사
    this->bAutoActive = SourceComponent->bAutoActive;
}

FVector USceneComponent::GetWorldRotation()
{
	if (AttachParent)
	{
		return FVector(AttachParent->GetLocalRotation() + GetLocalRotation());
	}
	else
		return GetLocalRotation();
}

FVector USceneComponent::GetWorldScale()
{
	if (AttachParent)
	{
		return FVector(AttachParent->GetWorldScale() + GetLocalScale());
	}
	else
		return GetLocalScale();
}

FVector USceneComponent::GetWorldLocation()
{
	if (AttachParent)
	{
		return FVector(AttachParent->GetWorldLocation() + GetLocalLocation());
	}
	else
		return GetLocalLocation();
}

FVector USceneComponent::GetLocalRotation()
{
	return JungleMath::QuaternionToEuler(QuatRotation);
}

void USceneComponent::SetRotation(FVector _newRot)
{
	RelativeRotation = _newRot;
	QuatRotation = JungleMath::EulerToQuaternion(_newRot);
}

void USceneComponent::SetupAttachment(USceneComponent* InParent)
{
    if (
        InParent != AttachParent                                  // 설정하려는 Parent가 기존의 Parent와 다르거나
        && InParent != this                                       // InParent가 본인이 아니고
        && InParent != nullptr                                    // InParent가 유효한 포인터 이며
        && (
            AttachParent == nullptr                               // AttachParent도 유효하며
            || !AttachParent->AttachChildren.Contains(this)  // 이미 AttachParent의 자식이 아닌 경우
        ) 
    ) {
        AttachParent = InParent;
        InParent->AttachChildren.AddUnique(this);
    }
}