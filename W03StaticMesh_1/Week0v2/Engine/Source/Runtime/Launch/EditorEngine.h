#pragma once
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "Core/HAL/PlatformType.h"
#include "D3D11RHI/GraphicDevice.h"
#include "Renderer/Renderer.h"
#include "Engine/ResourceMgr.h"

class UnrealEd;
class UImGuiManager;
class UWorld;
class FEditorViewportClient;
class SSplitterV;
class SSplitterH;
class SLevelEditor;
struct FWorldContext;

class UEditorEngine :public UObject 
{
    DECLARE_CLASS(UEditorEngine, UObject)

public:
    UEditorEngine() = default;

    static UEditorEngine* CreateEngine();

    virtual int32 PreInit();
    virtual int32 Init(HINSTANCE hInstance);
    virtual void Render();
    virtual void Tick(float DeltaSeconds);
    virtual void Exit();
    virtual float GetAspectRatio(IDXGISwapChain* swapChain) const;
    virtual void Input();

private:
    void WindowInit(HINSTANCE hInstance);

public:
    static FGraphicsDevice graphicDevice;
    static FRenderer renderer;
    static FResourceMgr resourceMgr;
    static uint32 TotalAllocationBytes;
    static uint32 TotalAllocationCount;


    HWND hWnd;

private:
    UImGuiManager* UIMgr;
    TArray<FWorldContext> WorldContexts;
    UWorld* GWorld;
    SLevelEditor* LevelEditor;
    UnrealEd* UnrealEditor;
    bool bTestInput = false;

public:
    UWorld* GetWorld() const { return GWorld; }
    SLevelEditor* GetLevelEditor() const { return LevelEditor; }
    UnrealEd* GetUnrealEditor() const { return UnrealEditor; }

};