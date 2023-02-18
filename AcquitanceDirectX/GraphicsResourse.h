#pragma once
#include "Graphics.hpp"

class Graphics;

class GraphicsResourse
{
protected:
	static ID3D11DeviceContext* GetContext(Graphics& gfx) noexcept;
	static ID3D11Device* GetDevice(Graphics& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(Graphics& gfx) noxnd;
};