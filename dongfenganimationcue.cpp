#include "dongfenganimationcue.h"
#include <vtkRenderWindow.h>

DongfengAnimationCue::DongfengAnimationCue()
{
    _updateStateFunc = nullptr;
    _startValue = 0;
    _endValue = 1;
    _startTime = 0;
    _endTime = 1;
    SetTimeModeToNormalized();
}

DongfengAnimationCue::~DongfengAnimationCue()
{
    _updateStateFunc = nullptr;
}

void DongfengAnimationCue::SetUpdateMethod(std::function<void(double)> updateStateFunc, double startValue, double endValue, double startTime, double endTime)
{
    _updateStateFunc = updateStateFunc;
    _startValue = startValue;
    _endValue = endValue;
    _startTime = startTime;
    _endTime = endTime;
    SetStartTime(startTime);
    SetEndTime(endTime);
}

void DongfengAnimationCue::StartCueInternal()
{
    _updateStateFunc(_startValue);
    vtkAnimationCue::StartCueInternal();
}

void DongfengAnimationCue::TickInternal(double currentTime, double deltaTime, double clockTime)
{
    _updateStateFunc((_endValue - _startValue) * currentTime + _startValue);
    vtkAnimationCue::TickInternal(currentTime, deltaTime, clockTime);
}

void DongfengAnimationCue::EndCueInternal()
{
    _updateStateFunc(_endValue);
    vtkAnimationCue::EndCueInternal();
}
