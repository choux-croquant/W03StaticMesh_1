#pragma once
#include "Core/HAL/PlatformType.h"
#include "D3D11RHI/GraphicDevice.h"

class UEditorEngine;

class FEngineLoop  
{
public:
    // 현재는 FEngineLoop가 Tick의 Timer 기능과
    // UEditorEngine의 Wrapping 기능만 하도록 하고 있음
    // Engine의 종류가 다양해질 경우, 공통 작업을 FEngineLoop에서 수행하는 등
    // 분리를 시키면 될 것 같다
    FEngineLoop();

    int32 PreInit();
    int32 Init(HINSTANCE hInstance);
    void Render();
    void Tick();
    void Exit();
    float GetAspectRatio(IDXGISwapChain* swapChain) const;
    void Input();


public:
    UEditorEngine* EditorEngine;

private:
    const int32 targetFPS = 60;
    bool bIsExit = false;
};
