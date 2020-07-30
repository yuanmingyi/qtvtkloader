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
    /// \brief GetTime: Get the animation time
    /// \return time: in seconds
    ///
    double GetTime() const { return _time; }

    ///
    /// \brief SetTime: Set the animation time
    /// \param time: in seconds
    ///
    void SetTime(double time) { _time = time; _speed = 0; SetEndTime(time); }

    ///
    /// \brief SetAnimationSpeed set the rate of change for the value of the key frame.
    /// Must be called before Add any update function
    /// \param speed: the rate of change for the value of the key frame
    ///
    void SetAnimationSpeed(double speed) { _speed = speed; _time = 0; }

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
    void SetRenderMethod(std::function<void(const std::string&)> method, const std::string& ctx = "") { _sceneObserver->SetRenderMethod(method, ctx); }

protected:
    DongfengAnimation();
    ~DongfengAnimation();

private:
    vtkSmartPointer<AnimationSceneObserver> _sceneObserver;
    double _time;
    double _speed;
};

#endif // ANIMATION_H
