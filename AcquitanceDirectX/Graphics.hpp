#pragma once
#include "MyWin.hpp"
#include <d3d11.h>
#include "ChiliException.hpp"
#include "DxgiInfoManager.hpp"
#include <wrl.h>
class Graphics
{
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
	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator = (const Graphics&) = delete;
	/*It means that you want to use the compiler-generated version of that function, so you don't need to specify a body.*/
	~Graphics() = default;
	void EndFrame();
	void ClearBuffer(float red, float green, float blue) noexcept;
	void DrawTriangle(float angle, float x, float y);
	
private:
#ifndef NDEBUG
	DxgiInfoManager infoManager;
#endif // !NDEBUG

	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr < IDXGISwapChain> pSwap ;
	Microsoft::WRL::ComPtr < ID3D11DeviceContext> pContext ;
	Microsoft::WRL::ComPtr < ID3D11RenderTargetView> pTarget ;
	Microsoft::WRL::ComPtr <ID3D11DepthStencilView> pDSV;

};

