#include "Camera.h"
#include "imgui/imgui.h"
#include "ChiliMath.h"
#include "Graphics.hpp"
#include "FrameCommander.h"

namespace dx = DirectX;
Camera::Camera(Graphics& gfx, const std::string& name_in, float pitch, float yaw, DirectX::XMFLOAT3 v, bool tethered)
	:
	name{ name_in },
	pos{ v },
	OrPos{ v },
	pitch{ pitch },
	yaw{ yaw },
	prj{ 1.0f, 3.0f / 4.0f, 0.5f, 400.0f },
	camInd{ gfx },
	frus{gfx,1.0f, 3.0f / 4.0f, 0.5f, 400.0f },
	tethered{tethered}
{
	camInd.SetPosition(pos);
	camInd.SetRotation({ pitch,yaw,0.0f });
	frus.SetPosition(pos);
	frus.SetRotation({ pitch,yaw,0.0f });
	frus.SetVertBuffer(gfx, prj.GetWidth(), prj.GetHeight(), prj.GetFar(), prj.GetNear());
	Reset();
}

void Camera::Submit(FrameComander& fc, size_t channel)
{
	if (showCamera)
	{
		camInd.Submit(fc, channel);
	}
	if (showFrustum)
	{
		frus.Submit(fc, channel);
	}
}

std::string Camera::GetName() const
{
	return name;
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	using namespace dx;
	dx::XMVECTOR dirVec = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	dx::XMVECTOR LookVector = dx::XMVector3Transform(dirVec, dx::XMMatrixRotationRollPitchYaw(pitch * 0.995f, yaw * 0.995f, 0.0f));
	dx::XMVECTOR camPos = dx::XMLoadFloat3(&pos);
	dx::XMVECTOR TargetVec = LookVector + camPos;
	return dx::XMMatrixLookAtLH(camPos, TargetVec, dx::XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ));

	
}

DirectX::XMMATRIX Camera::GetProjection() const noexcept
{
	return  prj.GetMatrix();
}

void Camera::SetPos(DirectX::XMFLOAT3 pos_in)
{
	pos = pos_in;
}

void Camera::SpawnWidgets() noexcept
{
	if (!tethered)
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f);
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f);
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f);
	}
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, -90.0f * 0.995f, 90.0f * 0.995f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
		prj.SpawnWidges();
		ImGui::Checkbox("ShowCameraIndicator", &showCamera);
		ImGui::Checkbox("ShowFrustum", &showFrustum);
		
	
}

DirectX::XMVECTOR Camera::GetPosition()
{
	return DirectX::XMLoadFloat3(&pos);
}

void Camera::Reset() noexcept
{
	pos = OrPos;
	pitch = 0.0f;
	yaw = 0.0f;
	
}

void  Camera::BindToGraphics(Graphics& gfx) const 
{
	gfx.SetProjection(prj.GetMatrix());
	gfx.SetCamera(GetMatrix());
	//frus.SetVertBuffer(gfx, prj.GetWidth(), prj.GetHeight(), prj.GetFar(), prj.GetNear());

}

void Camera::ApplyTranformations(Graphics& gfx)
{
	frus.SetVertBuffer(gfx, prj.GetWidth(), prj.GetHeight(), prj.GetFar(), prj.GetNear());
	if (showCamera)
	{
		camInd.SetRotation({ pitch, yaw, 0.0f });
		camInd.SetPosition(pos);
	
	}
	if (showFrustum) {
		frus.SetRotation({ pitch, yaw, 0.0f });
		frus.SetPosition(pos);
	}
}

void Camera::Rotate(const int dx, const int dy)
{
	yaw = wrap_angle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2, 0.995f * PI / 2);
	if (showCamera)
	{
		camInd.SetRotation({ pitch, yaw, 0.0f });
	}
	if (showFrustum) {
		frus.SetRotation({ pitch, yaw, 0.0f });
	}
}

void Camera::Translate(DirectX::XMFLOAT3 translation)
{
	dx::XMStoreFloat3(&translation, dx::XMVector3Transform(dx::XMLoadFloat3(&translation),
		dx::XMMatrixRotationRollPitchYaw(pitch* 0.995f, yaw * 0.995f, 0.0f)
		* dx::XMMatrixScaling(movingSpeed, movingSpeed, movingSpeed)));

	pos.x += translation.x;
	pos.y += translation.y;
	pos.z += translation.z;
	if (showCamera)
	{
		camInd.SetPosition(pos);
	}
	if (showFrustum)
	{
		frus.SetPosition(pos);
	}
}