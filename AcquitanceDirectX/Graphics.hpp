#pragma once
#include "MyWin.hpp"
#include <d3d11.h>
#include "ChiliException.hpp"
#include "DxgiInfoManager.hpp"
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include <random>
#include "ConditionalNoexcept.h"
#include "GraphicsResourse.h"


class DepthStencil;
class RenderTarget;
namespace Bind
{
	class Bindable;
}

class Graphics
{
	friend class  Bind::Bindable;
	friend class GraphicsResourse;
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
	public:
		using  HrException::HrException;
		const char* GetType() const noexcept override;
	};
public:
	Graphics(HWND hWnd, int width, int height);
	Graphics(const Graphics&) = delete;
	Graphics& operator = (const Graphics&) = delete;
	/*It means that you want to use the compiler-generated version of that function, so you don't need to specify a body.*/
	~Graphics() = default;
	void EndFrame();
	void BeginFrame(float red, float green, float blue) noexcept;
	void DrawIndexed(UINT count) noxnd;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetCamera(DirectX::FXMMATRIX cam) noexcept;
	DirectX::XMMATRIX GetCamera() const noexcept;
	void EnableImgui() noexcept;
	void DisableImgui() noexcept;
	bool IsImguiEnabled() const noexcept;
	UINT GetWidth()const noexcept;
	UINT GetHeight() const noexcept;
	void BindSwapBuffer() noexcept;
	void BindSwapBuffer(const DepthStencil& ds) noexcept;
	std::shared_ptr<RenderTarget> GetTarget() const;

private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif // !NDEBUG

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr < IDXGISwapChain> pSwap ;
	Microsoft::WRL::ComPtr < ID3D11DeviceContext> pContext ;
	std::shared_ptr<RenderTarget> pTarget;
	Microsoft::WRL::ComPtr <ID3D11DepthStencilView> pDSV;

private:
	bool imguiEnabled = true;
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	UINT width;
	UINT height;

};

