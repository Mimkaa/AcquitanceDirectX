#include "Mesh.h"
#include "FrameCommander.h"
namespace dx = DirectX;
void Mesh::Submit(FrameComander& frame, dx::FXMMATRIX accumulatedTranform) const noxnd
{
	dx::XMStoreFloat4x4(&transform, accumulatedTranform);
	Drawable::Submit(frame);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return XMLoadFloat4x4(&transform);
}
Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noexcept
	:
	Drawable(gfx, mat, mesh, scale)
{}