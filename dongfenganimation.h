#ifndef ANIMATION_H
#define ANIMATION_H

#include <vtkSmartPointer.h>
#include <vtkAnimationCue.h>
#include <vtkAnimationScene.h>

#include "animationsceneobserver.h"

////
/// \class DongfengAnimation
/// \brief The DongfengAnimation class
/// wraps a animation process
///
class DongfengAnimation : public vtkAnimationScene
{
public:
    static DongfengAnimation* New()
    {
        return new DongfengAnimation();
    }

    ///
    /// \brief Add a udpate method to invoke with the calculated linear value at each animation frame
    /// \param updateStateFunc: the method with a double parameter to add
    /// \param startValue: initial value at the start of the animation
    /// \param endValue: target value at the end of the animation
    /// \param startTime: start time (0-1) of the update method refering to the animation scene
    /// \param endTime: end time (0-1) of the update method refering to the animation scene
    ///
    void Add(std::function<void(double)> updateStateFunc, double startValue, double endValue, double startTime = 0, double endTime = 1);

    ///
    /// \brief set the render method to update the window at each animation frame
    /// \param method: the function to call at each animation frame
    ///
    void SetRenderMethod(std::function<void()> method) { _sceneObserver->SetRenderMethod(method); }

protected:
    DongfengAnimation();
    ~DongfengAnimation();

private:
    vtkSmartPointer<AnimationSceneObserver> _sceneObserver;
};

#endif // ANIMATION_H
