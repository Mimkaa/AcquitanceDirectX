#pragma once
#include <limits>
namespace Dcb
{
	class Buffer;
}
class TechniqueProbe
{
public:
	void SetTech(class Technique* tech_in)
	{
		pTech = tech_in;
		techIdx++;
		OnSetTech();
	}
	void SetStep(class Step* step_in)
	{
		pStep = step_in;
		stepIdx++;
		OnSetStep();
	}
	virtual bool VisitBuffer(class Dcb::Buffer& baf_in)
	{
		bufIdx++;
		return OnVisitBuffer(baf_in);
	}
protected:
	virtual void OnSetStep() {};
	virtual void OnSetTech() {};
	virtual bool OnVisitBuffer(class Dcb::Buffer& baf_in) = 0;
protected:
	class Technique* pTech;
	class Step* pStep;
	// after the max num it wraps around
	size_t techIdx = std::numeric_limits<size_t>::max();
	size_t stepIdx = std::numeric_limits<size_t>::max();
	size_t bufIdx = std::numeric_limits<size_t>::max();
};