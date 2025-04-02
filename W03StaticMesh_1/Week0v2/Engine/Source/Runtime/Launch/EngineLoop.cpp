#include "EngineLoop.h"
#include "ImGuiManager.h"
#include "World.h"
#include "Camera/CameraComponent.h"
#include "PropertyEditor/ViewportTypePanel.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/UnrealEd.h"
#include "UnrealClient.h"
#include "slate/Widgets/Layout/SSplitter.h"
#include "LevelEditor/SLevelEditor.h"
#include "GameFramework/Actor.h" 

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }
    int zDelta = 0;
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        if (wParam != SIZE_MINIMIZED)
        {
            //UGraphicsDevice 객체의 OnResize 함수 호출
            if (FEngineLoop::graphicDevice.SwapChain)
            {
                FEngineLoop::graphicDevice.OnResize(hWnd);
            }
            for (int i = 0; i < 4; i++)
            {
                if (GEngineLoop.GetLevelEditor())
                {
                    if (GEngineLoop.GetLevelEditor()->GetViewports()[i])
                    {
                        GEngineLoop.GetLevelEditor()->GetViewports()[i]->ResizeViewport(FEngineLoop::graphicDevice.SwapchainDesc);
                    }
                }
            }
        }
     Console::GetInstance().OnResize(hWnd);
    // ControlPanel::GetInstance().OnResize(hWnd);
    // PropertyPanel::GetInstance().OnResize(hWnd);
    // Outliner::GetInstance().OnResize(hWnd);
    // ViewModeDropdown::GetInstance().OnResize(hWnd);
    // ShowFlags::GetInstance().OnResize(hWnd);
        if (GEngineLoop.GetUnrealEditor())
        {
            GEngineLoop.GetUnrealEditor()->OnResize(hWnd);
        }
        ViewportTypePanel::GetInstance().OnResize(hWnd);
        break;
    case WM_MOUSEWHEEL:
        if (ImGui::GetIO().WantCaptureMouse)
            return 0;
        zDelta = GET_WHEEL_DELTA_WPARAM(wParam); // 휠 회전 값 (+120 / -120)
        if (GEngineLoop.GetLevelEditor())
        {
            if (GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->IsPerspective())
            {
                if (GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->GetIsOnRBMouseClick())
                {
                    GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->SetCameraSpeedScalar(
                        static_cast<float>(GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->GetCameraSpeedScalar() + zDelta * 0.01)
                    );
                }
                else
                {
                    GEngineLoop.GetLevelEditor()->GetActiveViewportClient()->CameraMoveForward(zDelta * 0.1f);
                }
            }
            else
            {
                FEditorViewportClient::SetOthoSize(-zDelta * 0.01f);
            }
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

FGraphicsDevice FEngineLoop::graphicDevice;
FRenderer FEngineLoop::renderer;
FResourceMgr FEngineLoop::resourceMgr;
uint32 FEngineLoop::TotalAllocationBytes = 0;
uint32 FEngineLoop::TotalAllocationCount = 0;

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

UWorld* FEngineLoop::DuplicateWorldForPIE(UWorld* SourceWorld)
{
    if (!SourceWorld)
        return nullptr;


    if (!EditorWorld)
    {
        EditorWorld = SourceWorld;
    }


    // 🌟 기존 월드 백업 (원본 보호)
    TSet<AActor*> BackupActors = SourceWorld->GetActors();

    // 🌟 새로운 월드 생성
    UWorld* PIEWorld = new UWorld();

    // 🌟 새로운 월드에 복사할 액터 목록
    TSet<AActor*> ClonedActors;

    for (AActor* SourceActor : BackupActors)
    {
        if (!SourceActor) 
            continue;

        // ✅ 새로운 액터 복제 (깊은 복사)
        AActor* ClonedActor = SourceActor->Duplicate();
        if (ClonedActor)
        {
            ClonedActors.Add(ClonedActor);
        }
    }

    // 🌟 복제된 액터들을 PIE 월드에 추가
    PIEWorld->SetPickedActor(nullptr);
    PIEWorld->SetActors(ClonedActors);
    PIEWorld->Initialize();

    return PIEWorld;
}

void FEngineLoop::StartEditorMode()
{
    if (!EditorWorld)
        return;  // 에디터 월드가 없으면 아무것도 하지 않음

    // 🌟 PIE 월드 정리
    if (GWorld && GWorld != EditorWorld)
    {
        // PIE 월드의 모든 액터를 정리
        TSet<AActor*> PIEActors = GWorld->GetActors();
        for (AActor* Actor : PIEActors)
        {
            if (Actor)
            {
                Actor->Destroy();
            }
        }
    }

    // 🌟 원래의 Editor World로 복원
    GWorld = EditorWorld;

    // 🌟 에디터 월드의 상태 복원 (필요한 경우)
    // ...

    // 백업 제거 (다음 PIE 세션을 위해)
    EditorWorld = nullptr;
}

void FEngineLoop::StartPIEMode()
{   
    GWorld = DuplicateWorldForPIE(GWorld);

}

void FEngineLoop::EndPIEMode()
{
   /* if (GWorld && GWorld->IsPIEWorld())
    {
        GWorld->CleanupWorld();
        delete GWorld;
    }

    GWorld = GEditor->GetEditorWorldContext().World();*/
}

void FEngineLoop::Exit()
{
    EditorEngine->Exit();
}

