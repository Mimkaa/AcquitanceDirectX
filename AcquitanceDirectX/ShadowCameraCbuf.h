#pragma once
#include "Camera.h"
#include "Bindable.h"
#include "BindableCommon.h"
namespace Bind {
	class ShadowCameraCbuf :public Bind::Bindable
	{
	public:
		struct Transform
		{
			DirectX::XMMATRIX ViewProj;
		};
		void SetCamera(const Camera& cam);
		void Bind(Graphics& gfx) noexcept override;
		ShadowCameraCbuf(Graphics& gfx, UINT slot = 3u);
		void Update(Graphics& gfx);
	private:
		const Camera* pCamera;
		std::unique_ptr<Bind::VertexConstantBuffer<Transform>> pVcbuf;

	};
}