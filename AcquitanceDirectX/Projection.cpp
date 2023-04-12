#include "Projection.h"
#include "imgui/imgui.h"
Projection::Projection(float width, float height, float near, float far)
	:
	width{ width },
	height{height},
	nearr{near},
	farr{far}
{}

DirectX::XMMATRIX Projection::GetMatrix() const
{
	return DirectX::XMMatrixPerspectiveLH(width, height, nearr, farr);
}

void Projection::SpawnWidges()
{
	ImGui::SliderFloat("width", &width, 0.5f, 2.0f, "%.3f");
	ImGui::SliderFloat("height", &height, 0.5f, 2.0f, "%.3f");
	ImGui::SliderFloat("near", &nearr, 0.1f, farr-0.01, "%.3f");
	ImGui::SliderFloat("far", &farr, nearr + 0.01f, 400.0f, "%.3f");
}