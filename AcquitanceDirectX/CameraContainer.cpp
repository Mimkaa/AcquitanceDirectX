#include "CameraContainer.h"
#include "Camera.h"
#include "imgui/imgui.h"
#include "Graphics.hpp"


void CameraContainer::AddCamera(std::unique_ptr<Camera> camera)
{
	cameras.push_back(std::move(camera));
}

void CameraContainer::Bind(Graphics& gfx) const
{
    GetCamera().BindToGraphics(gfx);
    
}

void CameraContainer::Submit(FrameComander& fc)
{
    for (int i = 0; i < cameras.size(); i++)
    {
        if (i != selected)
        {
            (*cameras[i]).Submit(fc);
        }
    }
}

Camera& CameraContainer::GetCamera() const
{
	return *cameras[selected];
}

void CameraContainer::ControlWindow() 
{
	ImGui::Begin("Model");
  
    if (ImGui::BeginCombo("Camera Select", GetCamera().GetName().c_str()))
    {
        for (int i = 0; i < cameras.size(); i++)
        {
            const bool is_selected = i == selected;
            
            if (ImGui::Selectable(cameras[i]->GetName().c_str(), is_selected))
                selected = i;
            
        }
        ImGui::EndCombo();
    }
    GetCamera().SpawnWidgets();
    
	ImGui::End();
}