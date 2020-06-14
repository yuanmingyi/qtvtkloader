#include "dongfenganimation.h"
#include "dongfenganimationcue.h"

DongfengAnimation::DongfengAnimation()
{
    _sceneObserver = vtkSmartPointer<AnimationSceneObserver>::New();
    AddObserver(vtkCommand::AnimationCueTickEvent, _sceneObserver);
    SetModeToRealTime();
    //SetModeToSequence();
    SetLoop(false);
    SetFrameRate(5);
    SetStartTime(0);
    SetEndTime(1);
}

DongfengAnimation::~DongfengAnimation()
{

}

void DongfengAnimation::SetRenderWindow(vtkRenderWindow* renderWindow)
{
    _sceneObserver->SetRenderWindow(renderWindow);
}

void DongfengAnimation::Add(std::function<void(double)> updateStateFunc, double startValue, double endValue, double startTime, double endTime)
{
    vtkNew<DongfengAnimationCue> cue;
    cue->SetUpdateMethod(updateStateFunc, startValue, endValue, startTime, endTime);
    AddCue(cue);
}
