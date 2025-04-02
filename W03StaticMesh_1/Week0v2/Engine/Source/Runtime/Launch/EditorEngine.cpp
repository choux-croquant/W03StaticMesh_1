#include "EditorEngine.h"
#include "ImGuiManager.h"
#include "World.h"
#include "Camera/CameraComponent.h"
#include "PropertyEditor/ViewportTypePanel.h"
#include "UnrealEd/EditorViewportClient.h"
#include "UnrealEd/UnrealEd.h"
#include "UnrealClient.h"
#include "slate/Widgets/Layout/SSplitter.h"
#include "LevelEditor/SLevelEditor.h"
#include "Engine/EngineTypes.h"

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
            if (UEditorEngine::graphicDevice.SwapChain)
            {
                UEditorEngine::graphicDevice.OnResize(hWnd);
            }
            for (int i = 0; i < 4; i++)
            {
                if (GEngineLoop.EditorEngine->GetLevelEditor())
                {
                    if (GEngineLoop.EditorEngine->GetLevelEditor()->GetViewports()[i])
                    {
                        GEngineLoop.EditorEngine->GetLevelEditor()->GetViewports()[i]->ResizeViewport(UEditorEngine::graphicDevice.SwapchainDesc);
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
        if (GEngineLoop.EditorEngine->GetUnrealEditor())
        {
            GEngineLoop.EditorEngine->GetUnrealEditor()->OnResize(hWnd);
        }
        ViewportTypePanel::GetInstance().OnResize(hWnd);
        break;
    case WM_MOUSEWHEEL:
        if (ImGui::GetIO().WantCaptureMouse)
            return 0;
        zDelta = GET_WHEEL_DELTA_WPARAM(wParam); // 휠 회전 값 (+120 / -120)
        if (GEngineLoop.EditorEngine->GetLevelEditor())
        {
            if (GEngineLoop.EditorEngine->GetLevelEditor()->GetActiveViewportClient()->IsPerspective())
            {
                if (GEngineLoop.EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetIsOnRBMouseClick())
                {
                    GEngineLoop.EditorEngine->GetLevelEditor()->GetActiveViewportClient()->SetCameraSpeedScalar(
                        static_cast<float>(GEngineLoop.EditorEngine->GetLevelEditor()->GetActiveViewportClient()->GetCameraSpeedScalar() + zDelta * 0.01)
                    );
                }
                else
                {
                    GEngineLoop.EditorEngine->GetLevelEditor()->GetActiveViewportClient()->CameraMoveForward(zDelta * 0.1f);
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

FGraphicsDevice UEditorEngine::graphicDevice;
FRenderer UEditorEngine::renderer;
FResourceMgr UEditorEngine::resourceMgr;
uint32 UEditorEngine::TotalAllocationBytes = 0;
uint32 UEditorEngine::TotalAllocationCount = 0;



UEditorEngine* UEditorEngine::CreateEngine()
{
    UEditorEngine* NewEngine = FObjectFactory::ConstructObject<UEditorEngine>();
    return NewEngine;
}

int32 UEditorEngine::PreInit()
{
    return 0;
}

int32 UEditorEngine::Init(HINSTANCE hInstance)
{
    /* must be initialized before window. */
    UnrealEditor = new UnrealEd();
    UnrealEditor->Initialize();

    WindowInit(hInstance);
    graphicDevice.Initialize(hWnd);
    renderer.Initialize(&graphicDevice);

    UIMgr = new UImGuiManager;
    UIMgr->Initialize(hWnd, graphicDevice.Device, graphicDevice.DeviceContext);

    resourceMgr.Initialize(&renderer, &graphicDevice);
    LevelEditor = new SLevelEditor();
    LevelEditor->Initialize();

    GWorld = UWorld::CreateWorld(EWorldType::Editor);

    return 0;
}

void UEditorEngine::Render()
{
    graphicDevice.Prepare();
    if (LevelEditor->IsMultiViewport())
    {
        std::shared_ptr<FEditorViewportClient> viewportClient = GetLevelEditor()->GetActiveViewportClient();
        for (int i = 0; i < 4; ++i)
        {
            LevelEditor->SetViewportClient(i);
            // graphicDevice.DeviceContext->RSSetViewports(1, &LevelEditor->GetViewports()[i]->GetD3DViewport());
            // graphicDevice.ChangeRasterizer(LevelEditor->GetActiveViewportClient()->GetViewMode());
            // renderer.ChangeViewMode(LevelEditor->GetActiveViewportClient()->GetViewMode());
            // renderer.PrepareShader();
            // renderer.UpdateLightBuffer();
            // RenderWorld();
            renderer.PrepareRender();
            renderer.Render(GetWorld(), LevelEditor->GetActiveViewportClient());
        }
        GetLevelEditor()->SetViewportClient(viewportClient);
    }
    else
    {
        // graphicDevice.DeviceContext->RSSetViewports(1, &LevelEditor->GetActiveViewportClient()->GetD3DViewport());
        // graphicDevice.ChangeRasterizer(LevelEditor->GetActiveViewportClient()->GetViewMode());
        // renderer.ChangeViewMode(LevelEditor->GetActiveViewportClient()->GetViewMode());
        // renderer.PrepareShader();
        // renderer.UpdateLightBuffer();
        // RenderWorld();
        renderer.PrepareRender();
        renderer.Render(GetWorld(), LevelEditor->GetActiveViewportClient());
    }
}

void UEditorEngine::Tick(float DeltaSeconds)
{
    Input();
    GWorld->Tick(DeltaSeconds);
    LevelEditor->Tick(DeltaSeconds);
    Render();
    UIMgr->BeginFrame();
    UnrealEditor->Render();

    Console::GetInstance().Draw();

    UIMgr->EndFrame();

    // Pending 처리된 오브젝트 제거
    GUObjectArray.ProcessPendingDestroyObjects();

    graphicDevice.SwapBuffer();
}

void UEditorEngine::Exit()
{
    LevelEditor->Release();
    GWorld->Release();
    delete GWorld;
    UIMgr->Shutdown();
    delete UIMgr;
    resourceMgr.Release(&renderer);
    renderer.Release();
    graphicDevice.Release();
}

float UEditorEngine::GetAspectRatio(IDXGISwapChain* swapChain) const
{
    DXGI_SWAP_CHAIN_DESC desc;
    swapChain->GetDesc(&desc);
    return static_cast<float>(desc.BufferDesc.Width) / static_cast<float>(desc.BufferDesc.Height);
}

void UEditorEngine::Input()
{
    if (GetAsyncKeyState('M') & 0x8000)
    {
        if (!bTestInput)
        {
            bTestInput = true;
            if (LevelEditor->IsMultiViewport())
            {
                LevelEditor->OffMultiViewport();
            }
            else
                LevelEditor->OnMultiViewport();
        }
    }
    else
    {
        bTestInput = false;
    }
}

void UEditorEngine::WindowInit(HINSTANCE hInstance)
{
    WCHAR WindowClass[] = L"JungleWindowClass";

    WCHAR Title[] = L"Game Tech Lab";

    WNDCLASSW wndclass = { 0 };
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.lpszClassName = WindowClass;

    RegisterClassW(&wndclass);

    hWnd = CreateWindowExW(
        0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 1000,
        nullptr, nullptr, hInstance, nullptr
    );
}
