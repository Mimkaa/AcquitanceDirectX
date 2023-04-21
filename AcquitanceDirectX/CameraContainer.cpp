#include "CameraContainer.h"
#include "Camera.h"
#include "imgui/imgui.h"
#include "Graphics.hpp"


void CameraContainer::AddCamera(std::shared_ptr<Camera> camera)
{
	cameras.push_back(std::move(camera));
}

void CameraContainer::Bind(Graphics& gfx) const
{
   
    GetActiveCamera().BindToGraphics(gfx);
    
    
}

void CameraContainer::Submit(FrameComander& fc, size_t channel)
{
    for (int i = 0; i < cameras.size(); i++)
    {
        if (i != active)
        {
            (*cameras[i]).Submit(fc, channel);
        }
    }
}

Camera& CameraContainer::GetActiveCamera() const
{
	return *cameras[active];
}

Camera& CameraContainer::GetControlCamera() const
{
    return *cameras[controled];
}

void CameraContainer::ControlWindow(Graphics& gfx) 
{
	ImGui::Begin("Model");
  
    if (ImGui::BeginCombo("Camera Active", GetActiveCamera().GetName().c_str()))
    {
        for (int i = 0; i < cameras.size(); i++)
        {
            const bool is_selected = i == active;
            
            if (ImGui::Selectable(cameras[i]->GetName().c_str(), is_selected))
                active = i;
            
        }
        ImGui::EndCombo();
    }
    if (ImGui::BeginCombo("Camera Controled",GetControlCamera().GetName().c_str()))
    {
        for (int i = 0; i < cameras.size(); i++)
        {
            const bool is_selected = i == controled;

            if (ImGui::Selectable(cameras[i]->GetName().c_str(), is_selected))
                controled = i;

        }
        ImGui::EndCombo();
    }
    GetControlCamera().SpawnWidgets();
    GetControlCamera().ApplyTranformations(gfx);
	ImGui::End();
}