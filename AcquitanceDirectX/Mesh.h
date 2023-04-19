#pragma once
#include "Drawable.h"
#include "Graphics.hpp"

class Material;
class FrameCommander;
struct aiMesh;

class Mesh :public Drawable
{
public:
	Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noexcept;
	//Mesh(Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>>binds_in);
	using Drawable::Drawable;
	DirectX::XMMATRIX GetTransformXM() const noexcept;

	void Submit(FrameComander& frame, DirectX::FXMMATRIX accumulatedTranform, size_t channel)  noxnd;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};

