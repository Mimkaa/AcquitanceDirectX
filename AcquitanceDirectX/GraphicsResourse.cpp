#include "GraphicsResourse.h"

ID3D11DeviceContext* GraphicsResourse::GetContext(Graphics& gfx) noexcept
{
	return gfx.pContext.Get();
}

ID3D11Device* GraphicsResourse::GetDevice(Graphics& gfx) noexcept
{
	return gfx.pDevice.Get();
}

DxgiInfoManager& GraphicsResourse::GetInfoManager(Graphics& gfx) noxnd
{
#ifndef NDEBUG
	return gfx.infoManager;
#else
	throw std::logic_error("YouFuckedUp! (tried to access gfx.infoManager in Release config)");
#endif
}