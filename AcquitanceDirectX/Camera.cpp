#include "Camera.h"
#include "imgui/imgui.h"
#include "ChiliMath.h"

namespace dx = DirectX;
Camera::Camera()
{
	Reset();
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

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &pos.x, -80.0f, 80.0f);
		ImGui::SliderFloat("Y", &pos.y, -80.0f, 80.0f);
		ImGui::SliderFloat("Z", &pos.z, -80.0f, 80.0f);
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &pitch, -90.0f * 0.995f, 90.0f * 0.995f);
		ImGui::SliderAngle("Yaw", &yaw, -180.0f, 180.0f);
		if (ImGui::Button("Reset"))
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	pos = { 0.0f, 0.0f, -5.0f };
	pitch = 0.0f;
	yaw = 0.0f;
	
}

void Camera::Rotate(const int dx, const int dy)
{
	yaw = wrap_angle(yaw + dx * rotationSpeed);
	pitch = std::clamp(pitch + dy * rotationSpeed, 0.995f * -PI / 2, 0.995f * PI / 2);
}

void Camera::Translate(DirectX::XMFLOAT3 translation)
{
	dx::XMStoreFloat3(&translation, dx::XMVector3Transform(dx::XMLoadFloat3(&translation),
		dx::XMMatrixRotationRollPitchYaw(pitch* 0.995f, yaw * 0.995f, 0.0f)
		* dx::XMMatrixScaling(movingSpeed, movingSpeed, movingSpeed)));

	pos.x += translation.x;
	pos.y += translation.y;
	pos.z += translation.z;

}