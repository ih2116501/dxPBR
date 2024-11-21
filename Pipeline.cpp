#include "Pipeline.h"

void Pipeline::SetPipeline(ComPtr<ID3D11DeviceContext> &context) {
    //context->ClearDepthStencilView(DepthStencilView.Get(),
    //                               D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
    //                               1.0f, 0);

    context->IASetInputLayout(InputLayout.Get());
    context->RSSetState(rs.Get());
    context->VSSetShader(vs.Get(), nullptr, 0);
    context->PSSetShader(ps.Get(), nullptr, 0);
    context->PSSetShaderResources(0, SRVs.size(), SRVs.data());
    
    //context->VSSetConstantBuffers(0, vsCbList.size(), vsCbList.data());
    //context->PSSetConstantBuffers(0, psCbList.size(), psCbList.data()); 
    context->OMSetDepthStencilState(DSS.Get(), 0);  

}
