#pragma once

class Job
{
public:
	Job(const class Drawable* drawable_in, const class Step* step_in);
	void Execute(class Graphics& gfx);
private:
	const class Step* step;
	const class Drawable* drawable;
};