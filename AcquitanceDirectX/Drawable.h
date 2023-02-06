#pragma once
#include "Graphics.hpp"
#include <DirectXMath.h>
#include <memory>
#include "Technique.h"

class TechniqueProbe;

namespace Bind
{
	class Bindable;
	class IndexBuffer;
	class VertexBuffer;
	class Topology;
}

class Drawable
{
	
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
	void Draw(Graphics& gfx) const noxnd;
	virtual ~Drawable() = default;

	void Submit(class FrameComander& frame) const noexcept;

	
	void Bind(Graphics& gfx) const;


	size_t GetIndexCount() const;
	
	
	void AddTechnique(Technique tecnique_in) noxnd;
	
	void Accept(TechniqueProbe& probe);

	
protected:
	std::shared_ptr<Bind::IndexBuffer> pIndexBuff;
	std::shared_ptr<Bind::VertexBuffer> pVertBuff;
	std::shared_ptr<Bind::Topology> pTopology;
	std::vector<Technique> techniques;

	
};
