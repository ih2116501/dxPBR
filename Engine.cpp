#include "Engine.h"
#include "D3DUtils.h"
#include "GeometryGenerator.h"
#include "ModelLoader.h"
#include "Pipeline.h"
#include <DirectXMath.h>
#include <directxtk/SimpleMath.h>
#include <memory>

using namespace DirectX::SimpleMath;

Engine::Engine()
    : mMainWindow(0), mScreenWidth(1280), mScreenHeight(720), mDevice(nullptr),
      mContext(nullptr), mSwapChain(nullptr), mBackBufferRTV(nullptr),
      mViewRot(Vector2(0.0f)), mViewport({}), mPrevMouseXY(Vector2(0.0f, 0.0f)),
      dMouse(Vector2(0.0f)), prevAxis(Vector3(0.0f)) {
    mGUIManager = std::make_shared<GUIManager>(mScreenWidth, mScreenHeight);
    mRenderManager =
        std::make_shared<RenderManager>(mScreenWidth, mScreenHeight);
    // mGUIManager = std::make_shared<GUIManager>();
    // mD3DUtils = std::make_shared<D3DUtils>();
}

Engine::~Engine() {

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    DestroyWindow(mMainWindow);

#if defined(DEBUG) || defined(_DEBUG)
    ID3D11Debug *d3dDebug = nullptr;
    mDevice.Get()->QueryInterface(__uuidof(ID3D11Debug), (void **)&d3dDebug);
    OutputDebugString(L"---------------------------print live object "
                      L"start----------------------------\n");
    d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_IGNORE_INTERNAL);
    OutputDebugString(L"---------------------------print live object "
                      L"end----------------------------\n");

    d3dDebug->Release();
#endif

    std::cout << "Engine closed.\n";
}

bool Engine::InitEngine() {

    bool ret = false;

    mGUIManager->InitWindows(mMainWindow);
    D3DUtils::CreateDevice(mDevice, mContext);
    std::cout << "device ptr : " << mDevice.Get() << std::endl;
    D3DUtils::CreateSwapChain(mDevice, mSwapChain, mMainWindow, mScreenWidth,
                              mScreenHeight);
    mGUIManager->InitGUI(mMainWindow, mDevice, mContext);

    D3DUtils::CreateViewport(mContext, mViewport, mScreenWidth, mScreenHeight);
    // D3DUtils::CreateRasterizerState(mDevice, mBasicRS);

    // mCamera = std::make_shared<Camera>();

    D3DUtils::CreateConstantBuffer(mDevice, mLight, mLightBuffer);

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
    return ret;
}

bool Engine::Run() {

    MeshData squareMesh = GeometryGenerator::CreateSquare();
    mScreenSquare = std::make_shared<Model>();
    mScreenSquare->Initialize(mDevice, mContext, squareMesh);

    // MeshData triangleMesh = GeometryGenerator::CreateTriangle();
    MeshData sphereMesh =
        GeometryGenerator::CreateSphere(10, 10, Vector2(2.0f, 1.0f));

    // MeshData box = GeometryGenerator::CreateBox(0.5f);
    std::string path = "./Assets/Textures/PBRTextures/";
    sphereMesh.albedoTextureFilename = path + "rusted-panels_albedo.png";
    sphereMesh.heightTextureFilename = path + "rusted-panels_height.png";
    sphereMesh.aoTextureFilename = path + "rusted-panels_ao.png";
    sphereMesh.metallicTextureFilename = path + "rusted-panels_metallic.png";
    sphereMesh.normalTextureFilename = path + "rusted-panels_normal-dx.png";
    sphereMesh.roughnessTextureFilename = path + "rusted-panels_roughness.png";

    // sphereMesh.albedoTextureFilename = path + "steelplate1_albedo.png";
    // sphereMesh.heightTextureFilename = path + "steelplate1_height.png";
    // sphereMesh.aoTextureFilename = path + "steelplate1_ao.png";
    // sphereMesh.metallicTextureFilename = path + "steelplate1_metallic.png";
    // sphereMesh.normalTextureFilename = path + "steelplate1_normal-dx.png";
    // sphereMesh.roughnessTextureFilename = path + "steelplate1_roughness.png";

    mMainModel = std::make_shared<Model>();
    // mMainModel->Initialize(mDevice, mContext, triangleMesh);

    std::string modelPath = "./Assets/DamagedHelmet/";
    ModelLoader modelLoader;
    modelLoader.Load(modelPath, "DamagedHelmet.gltf", false);
    std::vector<MeshData> md;
    md = modelLoader.GetMeshes();
    GeometryGenerator::NormalizeMesh(md, 3.0f);
    // mMainModel->Initialize(mDevice, mContext, sphereMesh);
    mMainModel->Initialize(mDevice, mContext, md);
    // this->objList.push_back(mMainModel);

    MeshData skyBoxMesh = GeometryGenerator::CreateBox(30.0f);
    mSkyBox = std::make_shared<Model>();
    mSkyBox->Initialize(mDevice, mContext, skyBoxMesh);
    this->objList.push_back(mSkyBox);

    MSG msg = {0};
    while (WM_QUIT != msg.message) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        else {
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
            // ImGui::SliderFloat2("viewrot", &mViewRot.x, -4.0f, 4.0f);

            ImGui::End();

            // render
            this->Update();
            this->Render();

            // render end
            ImGui::Render();
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            mSwapChain->Present(1, 0);
        }
    }
    return true;
}

bool Engine::Update() {
    mMainModel->mPixelConstData.useWireframe = mRenderManager->mUseWireframe;

    D3DUtils::UpdateBuffer(mDevice, mContext, mMainModel->mPixelConstData,
                           mMainModel->mPixelCB);
    D3DUtils::UpdateBuffer(mDevice, mContext, mLight, mLightBuffer);
    
    Vector2 currentMouseXY;
    constData.view = constData.view.Transpose();
    Matrix prevView = constData.view;
    if (mGUIManager->mLButtonDown) {
        if (mGUIManager->mLDragStart) {
            mGUIManager->mLDragStart = false;
            mPrevMouseXY = Vector2(float(mGUIManager->mMouseX),
                                   float(mGUIManager->mMouseY));
        } else {
            Vector3 axisZ = Vector3(0.0f, 0.0f, 1.0f);
            currentMouseXY = Vector2(float(mGUIManager->mMouseX),
                                     float(mGUIManager->mMouseY));
            dMouse = (mPrevMouseXY - currentMouseXY);

            //if (mGUIManager->mMouseMove) {

                Vector3 rotAxis =
                    axisZ.Cross(Vector3(dMouse.x / 100, -dMouse.y / 100, 0.0f));
                if (rotAxis != DirectX::XMVectorZero())
                    constData.view *=
                        Matrix::CreateTranslation(mEyePos) *
                        Matrix::CreateFromAxisAngle(rotAxis, 0.05f) *
                        Matrix::CreateTranslation(-mEyePos);
            //}
            mPrevMouseXY = currentMouseXY;
        }
    }
    constData.eyePos = Vector3::Transform(mEyePos, constData.view.Invert());
    constData.view = constData.view.Transpose();
    constData.vp = constData.proj * constData.view;

    D3DUtils::UpdateBuffer(mDevice, mContext, constData, mConstantBuffer);
    mGUIManager->mMouseMove = false;
    return true;
}

//bool Engine::Update() {
//    mMainModel->mPixelConstData.useWireframe = mRenderManager->mUseWireframe;
//    D3DUtils::UpdateBuffer(mDevice, mContext, mMainModel->mPixelConstData,
//                           mMainModel->mPixelCB);
//    D3DUtils::UpdateBuffer(mDevice, mContext, mLight, mLightBuffer);
//
//    constData.view = Matrix::CreateRotationX(mViewRot.x) *
//                     Matrix::CreateRotationY(mViewRot.y) *
//                     Matrix::CreateTranslation(-mEyePos);
//    constData.eyePos = Vector3::Transform(mEyePos, constData.view.Invert());
//    constData.view = constData.view.Transpose();
//    constData.vp = constData.proj * constData.view;
//
//    D3DUtils::UpdateBuffer(mDevice, mContext, constData, mConstantBuffer);
//    return true;
//}

bool Engine::Render() {
    std::vector<ID3D11Buffer *> cbList = {mConstantBuffer.Get(),
                                          mLightBuffer.Get()};
    std::vector<ID3D11RenderTargetView *> rtvs = {mBackBufferRTV.Get()};

    // common states
    mRenderManager->ClearFrame();

    mContext->VSSetConstantBuffers(0, 2, cbList.data());
    mContext->PSSetConstantBuffers(0, 2, cbList.data());

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
