#pragma once
#include <memory>
#include <vector>

class Camera;
class Graphics;
class FrameComander;

class CameraContainer
{
public:
	void ControlWindow() ;
	void Bind(Graphics& gfx) const;
	void AddCamera(std::unique_ptr<Camera> camera);
	Camera& GetCamera() const;
	CameraContainer() = default;
	~CameraContainer() = default;
	void Submit(FrameComander& fc);
private:
	std::vector<std::unique_ptr<Camera>> cameras;
	int selected = 0;
};