#include "dongfenganimation.h"
#include "dongfenganimationcue.h"

DongfengAnimation::DongfengAnimation()
{
    _scene = vtkSmartPointer<vtkAnimationScene>::New();
    _sceneObserver = vtkSmartPointer<AnimationSceneObserver>::New();
    _scene->AddObserver(vtkCommand::AnimationCueTickEvent, _sceneObserver);
    _scene->SetModeToRealTime();
    //_scene->SetModeToSequence();
    _scene->SetLoop(false);
    _scene->SetFrameRate(5);
    _scene->SetStartTime(0);
    _scene->SetEndTime(1);
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
    _scene->AddCue(cue);
}

void DongfengAnimation::Play()
{
    _scene->Play();
}

void DongfengAnimation::Stop()
{
    _scene->Stop();
}
