#include "Engine.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <iostream>

using namespace Microsoft::WRL;


int main() {
    ComPtr<ID3D11Device> device;
    Engine *engine = new Engine;
    engine->InitEngine(device);
    engine->Run();
    delete engine;

#if defined(DEBUG) || defined(_DEBUG)
    ID3D11Debug *d3dDebug = nullptr;
    device.Get()->QueryInterface(__uuidof(ID3D11Debug), (void **)&d3dDebug);
    OutputDebugString(L"---------------------------live object main"
                      L"start----------------------------\n");
    d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL |
                                      D3D11_RLDO_IGNORE_INTERNAL);
    OutputDebugString(L"---------------------------live object main "
                      L"end----------------------------\n");
    d3dDebug->Release();
#endif

    return 0;
}