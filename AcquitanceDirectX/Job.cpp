#include "Job.h"
#include "Step.h"
#include "Drawable.h"

Job::Job(const Drawable* drawable_in, const Step* step_in)
	:
	step{ step_in },
	drawable{ drawable_in }
{}

void Job::Execute(Graphics& gfx)
{
	step->Bind(gfx);
	drawable->Bind(gfx);
	gfx.DrawIndexed(drawable->GetIndexCount());

}
