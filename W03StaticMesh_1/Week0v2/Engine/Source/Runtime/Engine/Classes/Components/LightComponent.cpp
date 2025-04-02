#include "LightComponent.h"
#include "UBillboardComponent.h"
#include "Math/JungleMath.h"
#include "UnrealEd/PrimitiveBatch.h"
#include "UObject/ObjectFactory.h"
#include "Engine/Source/Runtime/CoreUObject/UObject/Casts.h"

ULightComponentBase::ULightComponentBase()
{
    // FString name = "SpotLight";
    // SetName(name);
    InitializeLight();
}

ULightComponentBase::~ULightComponentBase()
{
    delete texture2D;
}
ULightComponentBase* ULightComponentBase::Duplicate()
{
    // 🌟 현재 객체의 타입과 동일한 새 객체 생성
    ULightComponentBase* NewComponent = FObjectFactory::ConstructObject<ULightComponentBase>();

    // 🌟 속성 복사
    NewComponent->CopyPropertiesFrom(this);

    return NewComponent;
}
void ULightComponentBase::CopyPropertiesFrom(UObject* InSourceComponent)
{
    ULightComponentBase* SourceLight = Cast<ULightComponentBase>(InSourceComponent);
    if (!SourceLight)
        return;

    // 🌟 논리적 상태 복사
    this->color  = SourceLight->color;
    this->radius = SourceLight->radius;
    this->AABB   = SourceLight->AABB;

    // 🌟 컴포넌트의 고유 데이터 복사
    this->bAutoActive = SourceLight->bAutoActive;
}
void ULightComponentBase::SetColor(FVector4 newColor)
{
    color = newColor;
}

FVector4 ULightComponentBase::GetColor() const
{
    return color;
}

float ULightComponentBase::GetRadius() const
{
    return radius;
}

void ULightComponentBase::SetRadius(float r)
{
    radius = r;
}

void ULightComponentBase::InitializeLight()
{
    texture2D = new UBillboardComponent();
    texture2D->SetTexture(L"Assets/Texture/spotLight.png");
    texture2D->InitializeComponent();
    AABB.max = { 1.f,1.f,0.1f };
    AABB.min = { -1.f,-1.f,-0.1f };
    color = { 1,1,1,1 };
    radius = 5;
}

void ULightComponentBase::TickComponent(float DeltaTime)
{
    Super::TickComponent(DeltaTime);

    texture2D->TickComponent(DeltaTime);
    texture2D->SetLocation(GetWorldLocation());

}

int ULightComponentBase::CheckRayIntersection(FVector& rayOrigin, FVector& rayDirection, float& pfNearHitDistance)
{
    bool res =AABB.Intersect(rayOrigin, rayDirection, pfNearHitDistance);
    return res;
}

