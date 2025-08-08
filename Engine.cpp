#include "Engine.h"
#include "D3DUtils.h"
#include "GeometryGenerator.h"
#include "ModelLoader.h"
#include "Pipeline.h"
#include <directxtk/SimpleMath.h>
#include <memory>

using namespace DirectX::SimpleMath;
typedef enum { UVSphere, DamagedHelmet } MainModels;

Engine::Engine()
    : mMainWindow(0), mScreenWidth(1280), mScreenHeight(720), mDevice(nullptr),
      mContext(nullptr), mSwapChain(nullptr), mBackBufferRTV(nullptr),
      mViewRot(Vector2(0.0f)), mViewport({}), mPrevMouseXY(Vector2(0.0f, 0.0f)),
      dMouse(Vector2(0.0f)), mModelNum(0), mModelChangeFlag(true),
      mEnvChangeFlag(false) {
    mGUIManager = std::make_shared<GUIManager>(mScreenWidth, mScreenHeight);
    mRenderManager =
        std::make_shared<RenderManager>(mScreenWidth, mScreenHeight);
}

Engine::~Engine() {
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(mMainWindow);
    mContext->ClearState();
    mContext->Flush();
#if defined(DEBUG) || defined(_DEBUG)
    ID3D11Debug *d3dDebug = nullptr;
    mDevice.Get()->QueryInterface(__uuidof(ID3D11Debug), (void **)&d3dDebug);
    OutputDebugString(L"---------------------------print live object "
                      L"start----------------------------\n");
    d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL |
                                      D3D11_RLDO_IGNORE_INTERNAL);
    OutputDebugString(L"---------------------------print live object "
                      L"end----------------------------\n");

    d3dDebug->Release();
#endif
}

bool Engine::InitEngine(ComPtr<ID3D11Device> &device) {

    bool ret = false;

    mGUIManager->InitWindows(mMainWindow);
    D3DUtils::CreateDevice(mDevice, mContext);

    D3DUtils::CreateSwapChain(mDevice, mSwapChain, mMainWindow, mScreenWidth,
                              mScreenHeight);
    device = mDevice;
    mGUIManager->InitGUI(mMainWindow, mDevice, mContext);

    D3DUtils::CreateViewport(mContext, mViewport, mScreenWidth, mScreenHeight);
    mRenderManager->InitRenderer(mDevice, mContext, mSwapChain);

    // init constant buffer
    constData.proj = DirectX::XMMatrixPerspectiveFovLH(
        DirectX::XMConvertToRadians(80.0), float(mScreenWidth) / mScreenHeight,
        0.01f,
        100.0f); // todo : move to camera::set

    constData.proj = constData.proj.Transpose();
    mEyePos = Vector3(0.0f, 0.0f, -3.0f);

    constData.view = Matrix::CreateRotationX(mViewRot.x) *
                     Matrix::CreateRotationY(mViewRot.y) *
                     Matrix::CreateTranslation(-mEyePos);
    constData.eyePos = Vector3::Transform(mEyePos, constData.view.Invert());
    constData.view = constData.view.Transpose();
    constData.vp = constData.proj * constData.view;

    D3DUtils::CreateConstantBuffer(mDevice, constData, mConstantBuffer);

    MeshData squareMesh = GeometryGenerator::CreateSquare();
    mScreenSquare = std::make_shared<Model>();
    mScreenSquare->Initialize(mDevice, mContext, squareMesh);
    mScreenSquare->mPixelConstData.tonemap = 0;
    mMainModel = std::make_shared<Model>();

    MeshData skyBoxMesh = GeometryGenerator::CreateBox(50.0f);
    mSkyBox = std::make_shared<Model>();
    mSkyBox->Initialize(mDevice, mContext, skyBoxMesh);
    this->objList.push_back(mSkyBox);

    return ret;
}

bool Engine::Run() {

    MSG msg = {0};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        else {
            // clang-format off
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();

            ImGui::NewFrame();
            ImGui::Begin("Settings");
            ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
            ImGui::SetWindowSize(ImVec2(230, 210));
            ImGui::Text("Average %.3f ms/frame (%.1f FPS)",
                        1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);

            ImGui::Checkbox("wireframe", &mRenderManager->mUseWireframe);

            // select skybox
            const char *envs[] = {"room", "qwantani"};
            static int currentEnv = 0;
            mEnvChangeFlag = ImGui::Combo("Env", &mRenderManager->envNum, envs, IM_ARRAYSIZE(envs));

            // select model
            const char *models[] = {"UVSphere", "DamagedHelmet"};
            static int currentModelNum = 0; 
            int prevItemNum = currentModelNum;
            ImGui::Combo("model", &currentModelNum, models, IM_ARRAYSIZE(models));
            mModelNum = currentModelNum;
            if (prevItemNum != currentModelNum) {
                mModelChangeFlag = true;
            } 

            // roughness intensity
            ImGui::SliderFloat("Roughness", &mMainModel->mPixelConstData.rough_intensity, 1, 10);

            // select tonemapper
            const char *mappers[] = {"Linear", "Reinhard", "flimic", "unch2"};
            //int prevMapperNum = currentMapperNum;
            ImGui::Combo("tonemap", &mScreenSquare->mPixelConstData.tonemap, mappers, IM_ARRAYSIZE(mappers));

            // light control
            ImGui::SliderFloat3("lightPosX", &mMainModel->mPixelConstData.light.position.x, -4.0f, 4.0f);
            ImGui::ColorPicker3("lightColor", &mMainModel->mPixelConstData.light.color.x);
            
            ImGui::InputInt("option", &mMainModel->mPixelConstData.tonemap);

            ImGui::End();

            // render
            this->Update();
            this->Render();

            // render end
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            mSwapChain->Present(1, 0);
            // clang-format on
        }
    }
    return true;
}

bool Engine::Update() {
    if (mModelChangeFlag) {
        mModelChangeFlag = false;
        if (mModelNum == UVSphere) {
            MeshData sphereMesh =
                GeometryGenerator::CreateSphere(100, 100, Vector2(1.0f, 1.0f));
            // GeometryGenerator::CreateSphere(100, 100, Vector2(4.0f, 2.5f));

            // https://cc0-textures.com/t/cc0t-metal-004
            const std::string path = "./Assets/Textures/PBRTextures/";
            sphereMesh.albedoTextureFilename =
                path + "Metal004_4K-PNG_Color.png";
            sphereMesh.heightTextureFilename =
                path + "Metal004_4K-PNG_Displacement.png";
            sphereMesh.aoTextureFilename = path + "Metal004_4K-PNG_AO.png";
            sphereMesh.metallicTextureFilename =
                path + "Metal004_4K-PNG_Metalness.png";
            sphereMesh.normalTextureFilename =
                path + "Metal004_4K-PNG_NormalDX.png";
            sphereMesh.roughnessTextureFilename =
                path + "Metal004_4K-PNG_Roughness.png";
            mMainModel->Initialize(mDevice, mContext, sphereMesh);

        } else if (mModelNum == DamagedHelmet) {
            const std::string modelPath = "./Assets/DamagedHelmet/";
            ModelLoader modelLoader;
            modelLoader.Load(modelPath, "DamagedHelmet.gltf", false);
            std::vector<MeshData> md;
            md = modelLoader.GetMeshes();
            GeometryGenerator::NormalizeMesh(md, 3.0f);
            mMainModel->Initialize(mDevice, mContext, md);
            mMainModel->mPixelConstData.useMetallicRoughness = 1;
        }
    }
    mMainModel->mPixelConstData.useWireframe = mRenderManager->mUseWireframe;

    if (mEnvChangeFlag)
        mRenderManager->UpdateSkybox();

    D3DUtils::UpdateBuffer(mDevice, mContext, mMainModel->mPixelConstData,
                           mMainModel->mPixelCB);
    D3DUtils::UpdateBuffer(mDevice, mContext, mScreenSquare->mPixelConstData,
                           mScreenSquare->mPixelCB);

    Vector2 currentMouseXY;
    const auto &io = ImGui::GetIO();
    if (!io.WantCaptureMouse) {
        constData.view = constData.view.Transpose();
        if (mGUIManager->mLButtonDown) {
            if (mGUIManager->mLDragStart) {
                mGUIManager->mLDragStart = false;
                mPrevMouseXY = Vector2(float(mGUIManager->mMouseX),
                                       float(mGUIManager->mMouseY));
            } else {
                currentMouseXY = Vector2(float(mGUIManager->mMouseX),
                                         float(mGUIManager->mMouseY));
                dMouse += (mPrevMouseXY - currentMouseXY);
                constData.view = Matrix::CreateRotationY(dMouse.x / 150) *
                                 Matrix::CreateRotationX(dMouse.y / 150) *
                                 Matrix::CreateTranslation(-mEyePos);
                mPrevMouseXY = currentMouseXY;
            }
        }
        if (mGUIManager->mMouseWheel) {
            constData.view *= Matrix::CreateTranslation(mEyePos);
            mGUIManager->mMouseWheel = false;
            int dir = (mGUIManager->mDWheel > 0) - (mGUIManager->mDWheel < 0);
            mEyePos += Vector3(0.0f, 0.0f, dir * 0.3);
            constData.view *= Matrix::CreateTranslation(-mEyePos);
        }

        constData.eyePos = Vector3::Transform(mEyePos, constData.view.Invert());
        constData.view = constData.view.Transpose();
        constData.vp = constData.proj * constData.view;
    }

    D3DUtils::UpdateBuffer(mDevice, mContext, constData, mConstantBuffer);
    return true;
}

bool Engine::Render() {
    std::vector<ID3D11Buffer *> cbList = {mConstantBuffer.Get()};
    std::vector<ID3D11RenderTargetView *> rtvs = {mBackBufferRTV.Get()};

    // common states
    mRenderManager->ClearFrame();

    mContext->VSSetConstantBuffers(0, cbList.size(), cbList.data());
    mContext->PSSetConstantBuffers(0, cbList.size(), cbList.data());

    // render dafault objects
    mRenderManager->RenderObjects();
    mMainModel->Render(mContext); // objList[0]->Render(mContext);

    // render skybox
    mRenderManager->RenderSkybox();
    mSkyBox->Render(mContext);

    // render screen
    mRenderManager->RenderScreen();
    mScreenSquare->Render(mContext);

    return 0;
}
