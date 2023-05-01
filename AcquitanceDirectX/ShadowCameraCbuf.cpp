#include "ShadowCameraCbuf.h"

namespace Bind
{
	ShadowCameraCbuf::ShadowCameraCbuf(Graphics& gfx, UINT slot)
		
	{
		pVcbuf = std::make_unique <Bind::VertexConstantBuffer<Transform>>(gfx, slot);
	}

	void ShadowCameraCbuf::SetCamera(const Camera& cam)
	{
		pCamera = &cam;
	}

	void ShadowCameraCbuf::Bind(Graphics& gfx) noexcept
	{
		pVcbuf->Bind(gfx);
	}

	void ShadowCameraCbuf::Update(Graphics& gfx)
	{
		Transform t
		{
			DirectX::XMMatrixTranspose(pCamera->GetMatrix() * pCamera->GetProjection())
		};
		pVcbuf->Update(gfx, t);
	}
}