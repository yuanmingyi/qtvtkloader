#ifndef ANIMATION_H
#define ANIMATION_H

#include <vtkSmartPointer.h>
#include <vtkAnimationCue.h>
#include <vtkAnimationScene.h>

#include "animationsceneobserver.h"

class DongfengAnimation : public vtkAnimationScene
{
public:
    static DongfengAnimation* New()
    {
        return new DongfengAnimation();
    }

    void Add(std::function<void(double)> updateStateFunc, double startValue, double endValue, double startTime = 0, double endTime = 1);
    void SetRenderMethod(std::function<void()> method) { _sceneObserver->SetRenderMethod(method); }

protected:
    DongfengAnimation();
    ~DongfengAnimation();

private:
    vtkSmartPointer<AnimationSceneObserver> _sceneObserver;
};

#endif // ANIMATION_H
