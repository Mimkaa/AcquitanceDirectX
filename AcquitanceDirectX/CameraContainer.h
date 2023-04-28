#pragma once
#include <memory>
#include <vector>
#include <string>

class Camera;
class Graphics;
class FrameComander;

class CameraContainer
{
public:
	void ControlWindow(Graphics& gfx) ;
	void Bind(Graphics& gfx) const;
	void AddCamera(std::shared_ptr<Camera> camera);
	Camera& GetActiveCamera() const;
	Camera& GetControlCamera() const;
	Camera& GetByName(std::string name) const;
	CameraContainer() = default;
	~CameraContainer() = default;
	void Submit(FrameComander& fc, size_t channel);
	Camera* operator->() const
	{
		return cameras[controled].get();
	}
private:
	std::vector<std::shared_ptr<Camera>> cameras;
	int active = 0;
	int controled = 0;
};