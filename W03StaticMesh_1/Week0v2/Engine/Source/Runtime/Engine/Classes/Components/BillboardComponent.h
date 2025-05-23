#pragma once
#include "PrimitiveComponent.h"
#include "Engine/Classes/Engine/Texture.h"

class UBillboardComponent : public UPrimitiveComponent
{
    DECLARE_CLASS(UBillboardComponent, UPrimitiveComponent)
    
public:
    UBillboardComponent();
    virtual ~UBillboardComponent() override;

    virtual void InitializeComponent() override;
    virtual void TickComponent(float DeltaTime) override;
    virtual int CheckRayIntersection(
        FVector& rayOrigin,
        FVector& rayDirection, float& pfNearHitDistance
    ) override;

    void SetTexture(FWString _fileName);
    FMatrix CreateBillboardMatrix();

    ID3D11Buffer* vertexTextureBuffer;
    ID3D11Buffer* indexTextureBuffer;
    UINT numVertices;
    UINT numIndices;
    float finalIndexU = 0.0f;
    float finalIndexV = 0.0f;
    std::shared_ptr<FTexture> Texture;
    void CreateQuadTextureVertexBuffer();
protected:

    USceneComponent* m_parent = nullptr;
    bool CheckPickingOnNDC(const TArray<FVector>& checkQuad, float& hitDistance);

private:
};
