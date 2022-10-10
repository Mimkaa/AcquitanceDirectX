#include "Graphics.hpp"
#include <sstream>
#include <d3dCompiler.h>
namespace wrl = Microsoft::WRL;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")
// graphics exception checking/throwing macros (some with dxgi infos)
#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_NOINFO(hrcall) if( FAILED( hr = (hrcall) ) ) throw Graphics::HrException( __LINE__,__FILE__,hr )

#ifndef NDEBUG
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO(hrcall) infoManager.Set(); if( FAILED( hr = (hrcall) ) ) throw GFX_EXCEPT(hr)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr),infoManager.GetMessages() )
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}
#else
#define GFX_EXCEPT(hr) Graphics::HrException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException( __LINE__,__FILE__,(hr) )
#define GFX_THROW_INFO_ONLY(call) (call)
#endif
Graphics::Graphics(HWND hWnd) {
    DXGI_SWAP_CHAIN_DESC sd = { 0 };
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    UINT swapCreateFlags = 0u;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif


    // for checking results of d3d functions
    // it can be eihter 0 -> success, 1 -> feilure
    //0 by default
    HRESULT hr;
    // create device and front/back buffer, swap chain, rendering context
    GFX_THROW_INFO(
        D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            swapCreateFlags,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &sd,
            &pSwap,
            &pDevice,
            nullptr,
            &pContext
        ));
    // gain access to texture subresource(interface) in swap chain (back buffer)
    wrl::ComPtr<ID3D11Resource> pBackBuffer;
    // & frees the pointer if I only need an address I should use GetAddressOf()
    // also it supports move semantics
    GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    GFX_THROW_INFO(pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &pTarget));

}



void Graphics::EndFrame()
{
    HRESULT hr = 0;
#ifndef NDEBUG
    infoManager.Set();
#endif
    if (FAILED(hr == pSwap->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED)
        {
            throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
        }
        else
        {
           throw GFX_EXCEPT(hr);
        }
    }
    pSwap->Present(1u, 0u);
}

void Graphics::ClearBuffer(float red, float green, float blue) noexcept
{
    
    const float color[] = { red, green, blue, 1.0f };
    pContext->ClearRenderTargetView(pTarget.Get(), color);
    
}



void Graphics::DrawTriangle()
{
    HRESULT hr;
    namespace wrl = Microsoft::WRL;

    struct Vertex
    {
        // anonimous structure
        struct
        {
            float x;
            float y;
        } pos;
        struct
        {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        }color;
    };
    // create vertex buffer (triangle at the center of the screen)
    Vertex vertices[] =
    {
        { 0.0f, 0.5f, 255, 0, 0, 0 },
        { 0.5f, -0.5f, 0, 255, 0, 0  },
        { -0.5f, -0.5f, 0, 0, 255, 0  },
        {-0.3, 0.3f,0,255,0,0},
        {0.3f,0.3,0,0,255,0},
        {0.0f,-1.8f,255,0,0,0},
        
        
        
    };
    vertices[0].color.g = 255;


    wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
    D3D11_BUFFER_DESC bd = { 0 };
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.CPUAccessFlags = 0u;
    bd.MiscFlags = 0u;
    bd.ByteWidth = sizeof(vertices);
    bd.StructureByteStride = sizeof(Vertex);
    D3D11_SUBRESOURCE_DATA sb = { 0 };
    sb.pSysMem = vertices;
    GFX_THROW_INFO(pDevice->CreateBuffer(&bd, &sb, &pVertexBuffer));

    // Bind vertex buffer to pipeline
    UINT stride = sizeof(Vertex);
    UINT offset = 0u;
    pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);

    // create index buffer
    const unsigned short indices[] =
    {
        0,1,2,
        0,2,3,
        0,4,1,
        2,1,5,
    };
    wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
    D3D11_BUFFER_DESC ibd = { 0 };
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.CPUAccessFlags = 0u;
    ibd.MiscFlags = 0u;
    ibd.ByteWidth = sizeof(indices);
    ibd.StructureByteStride = sizeof(const unsigned short);
    D3D11_SUBRESOURCE_DATA isb = { 0 };
    isb.pSysMem = indices;
    GFX_THROW_INFO(pDevice->CreateBuffer(&ibd, &isb, &pIndexBuffer));
    // bind index buffer
    pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
   
   
    // create pixel shader
    wrl::ComPtr<ID3D11PixelShader> pPixelShader;
    wrl::ComPtr<ID3DBlob> pBlob;
    GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
    GFX_THROW_INFO(pDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
    // bind pixel shader
    pContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

    // create vertex shader
    wrl::ComPtr<ID3D11VertexShader> pVertexShader;
    GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
    GFX_THROW_INFO(pDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
    // bind vertex shader
    pContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

    
    // input (vertex) layout (2d position only)
    wrl::ComPtr<ID3D11InputLayout> pInputLayout;
    const D3D11_INPUT_ELEMENT_DESC ied[] =
    {
        {"Pubes",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0},
        {"Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,8u,D3D11_INPUT_PER_VERTEX_DATA,0}
    };

    GFX_THROW_INFO(pDevice->CreateInputLayout(
        ied, (UINT)std::size(ied),
        pBlob->GetBufferPointer(),
        pBlob->GetBufferSize(),
        &pInputLayout

    ));
    //bind vertex layout
    // AI stands for input assembler
    pContext->IASetInputLayout(pInputLayout.Get());
    
    // bind render target
    pContext->OMSetRenderTargets(1u,pTarget.GetAddressOf(), nullptr);

    // Set primitive topology to triangle list (group of 3 verticies)
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    // configure viewport
    D3D11_VIEWPORT vp;
    vp.Width = 400;
    vp.Height = 300;
    vp.MinDepth = 0;
    vp.MaxDepth = 1;
    vp.TopLeftX = 100;
    vp.TopLeftY = 100;
    pContext->RSSetViewports(1u, &vp);
    
    GFX_THROW_INFO_ONLY(pContext->DrawIndexed((UINT)std::size(indices), 0u,0u));
}

Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs ) noexcept
    :
    Exception(line, file),
    hr(hr)
{
    // join all info messages with newlines into single string
    for (const auto& m : infoMsgs)
    {
        info += m;
        info.push_back('\n');
    }
    // remove final newline if exists
    if (!info.empty())
    {
        info.pop_back();
    }
}

const char* Graphics::HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
        << std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
        << "[Error String] " << GetErrorString() << std::endl
        << "[Description] " << GetErrorDescription() << std::endl;
    if (!info.empty())
    {
        oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    }
        oss<< GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
    return "Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
    return hr;
}

std::string Graphics::HrException::GetErrorString() const noexcept
{
    return "exception name";
    
}
std::string Graphics::HrException::GetErrorInfo() const noexcept
{
    return info;
}
std::string Graphics::HrException::GetErrorDescription() const noexcept
{
    HRESULT a_hResult = hr; // Wherever you get the HRESULT from...

    LPVOID a_pvMsgBuf;
    LPCTSTR a_pszMsg = NULL;
    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        (DWORD)a_hResult,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR)&a_pvMsgBuf, 0, NULL))
    {
        a_pszMsg = (LPTSTR)a_pvMsgBuf;
        // Do something with the error message.
        LocalFree(a_pvMsgBuf);
        std::wstring w = a_pszMsg;
        std::string s = std::string(w.begin(), w.end());
        return s;
    }
}


const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
    return "Chili Graphics Exception [Device Removed] (GFX_DEVICE_REMOVED_EXCEPT)";
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
    :
    Exception(line, file)
{
    // join all info messages with newlines into single string
    for (const auto& m : infoMsgs)
    {
        info += m;
        info.push_back('\n');
    }
    // remove final newline if exists
    if (!info.empty())
    {
        info.pop_back();
    }
}


const char* Graphics::InfoException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
    return "Chili Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
    return info;
}