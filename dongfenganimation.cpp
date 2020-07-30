#include "dongfenganimation.h"
#include "dongfenganimationcue.h"

DongfengAnimation::DongfengAnimation()
{
    _sceneObserver = vtkSmartPointer<AnimationSceneObserver>::New();
    AddObserver(vtkCommand::AnimationCueTickEvent, _sceneObserver);
    AddObserver(vtkCommand::EndAnimationCueEvent, _sceneObserver);
    SetModeToRealTime();
    //SetModeToSequence();
    SetLoop(false);
    SetFrameRate(5);
    SetStartTime(0);
    _speed = 2.0;
    _time = 0;
}

DongfengAnimation::~DongfengAnimation()
{

}

void DongfengAnimation::Add(std::function<void(double)> updateStateFunc, double startValue, double endValue, double startTime, double endTime)
{
    vtkNew<DongfengAnimationCue> cue;
    cue->SetUpdateMethod(updateStateFunc, startValue, endValue, startTime, endTime);
    AddCue(cue);
    if (_speed > 0 && endTime > startTime) {
        auto fulltime = (endValue - startValue) / _speed / (endTime - startTime);
        if (fulltime > _time) {
            _time = fulltime;
            SetEndTime(_time);
        }
    }
}
