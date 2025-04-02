#include "EngineLoop.h"
#include "EditorEngine.h"

FEngineLoop::FEngineLoop()
{
    EditorEngine = UEditorEngine::CreateEngine();
}

int32 FEngineLoop::PreInit()
{
    return EditorEngine->PreInit();
}

int32 FEngineLoop::Init(HINSTANCE hInstance)
{
    return EditorEngine->Init(hInstance);
}


void FEngineLoop::Render()
{
    EditorEngine->Render();
}

void FEngineLoop::Tick()
{
    LARGE_INTEGER frequency;
    const double targetFrameTime = 1000.0 / targetFPS; // 한 프레임의 목표 시간 (밀리초 단위)

    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER startTime, endTime;
    double elapsedTime = 1.0;

    while (bIsExit == false)
    {
        QueryPerformanceCounter(&startTime);

        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg); // 키보드 입력 메시지를 문자메시지로 변경
            DispatchMessage(&msg);  // 메시지를 WndProc에 전달

            if (msg.message == WM_QUIT)
            {
                bIsExit = true;
                break;
            }
        }
        EditorEngine->Tick(elapsedTime);
        do
        {
            Sleep(0);
            QueryPerformanceCounter(&endTime);
            elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
        }
        while (elapsedTime < targetFrameTime);
    }
}

float FEngineLoop::GetAspectRatio(IDXGISwapChain* swapChain) const
{
    return EditorEngine->GetAspectRatio(swapChain);
}

void FEngineLoop::Input()
{
    EditorEngine->Input();
}

void FEngineLoop::Exit()
{
    EditorEngine->Exit();
}

