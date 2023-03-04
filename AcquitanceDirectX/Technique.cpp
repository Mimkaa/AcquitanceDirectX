#include "Technique.h"
#include "FrameCommander.h"
#include "Drawable.h"

void Technique::Submit(FrameComander& frame, const Drawable& drawable) 
{
	if (active)
	{
		for (auto& s : steps)
		{
			s.Submit(frame, drawable);
		}
	}
}

void Technique::InitializeParentReference(const Drawable& drawable) noexcept
{
	for (auto& s : steps)
	{
		s.InitializeParentReference(drawable);
	}
}