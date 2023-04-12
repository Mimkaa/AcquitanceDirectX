#pragma once
#include <memory>
#include <vector>

class Camera;
class Graphics;
class FrameComander;

class CameraContainer
{
public:
	void ControlWindow(Graphics& gfx) ;
	void Bind(Graphics& gfx) const;
	void AddCamera(std::unique_ptr<Camera> camera);
	Camera& GetCamera() const;
	Camera& GetControlCamera() const;
	CameraContainer() = default;
	~CameraContainer() = default;
	void Submit(FrameComander& fc);
	Camera* operator->() const
	{
		return cameras[controled].get();
	}
private:
	std::vector<std::unique_ptr<Camera>> cameras;
	int active = 0;
	int controled = 0;
};