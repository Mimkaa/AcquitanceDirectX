#include "Drawable.h"
#include "BindableCommon.h"
#include "BindableCodex.h"
using namespace Bind;

void Drawable::Bind(Graphics& gfx) const
{
	pVertBuff->Bind(gfx);
	pTopology->Bind(gfx);
	pIndexBuff->Bind(gfx);
}

size_t Drawable::GetIndexCount() const
{
	auto ret = pIndexBuff->GetCount();
	return ret;
}

void Drawable::Submit(class FrameComander& frame) const noexcept
{
	for (auto& t : techniques)
	{
		t.Submit(frame, *this);
	}
}

void Drawable::AddTechnique(Technique tecnique_in) noxnd
{
	tecnique_in.InitializeParentReference(*this);
	techniques.push_back(std::move(tecnique_in));
}

void Drawable::Accept(TechniqueProbe& probe)
{
	
	for (auto& t : techniques)
	{
		probe.SetTech(&t);
		t.Accept(probe);
	}
}


