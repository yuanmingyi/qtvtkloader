#ifndef CAMERAANIMATION_H
#define CAMERAANIMATION_H

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkAnimationCue.h>
#include <vtkAnimationScene.h>
#include <functional>
#include "animationsceneobserver.h"

struct CameraInfo
{
    double Position[3];
    double FocalPoint[3];
    double ViewUp[3];
    double ViewAngle;
};

class CameraAnimation : public vtkAnimationScene
{
public:
    static CameraAnimation* New()
    {
        return new CameraAnimation();
    }

    ///
    /// \brief Play: setup the animation and play at once
    /// \param renderMethod: render method for updating the animation frames
    /// \param renderer: the render which controls the camera
    /// \param start: the start position and pose information of the camera
    /// \param end: the end position and pose information of the camera
    /// \param time: animation lasting time (in second)
    ///
    void Play(std::function<void(const std::string&)> renderMethod, vtkRenderer* renderer, const CameraInfo& start, const CameraInfo& end, double speed = 1);

protected:
    CameraAnimation();

private:
    vtkSmartPointer<AnimationSceneObserver> _sceneObserver;
};

class CameraAnimationCue : public vtkAnimationCue
{
public:
    static CameraAnimationCue* New()
    {
        return new CameraAnimationCue();
    }

    void Setup(vtkRenderer* renderer, const CameraInfo& startCamera, const CameraInfo& endCamera);

protected:
    CameraAnimationCue();
    virtual ~CameraAnimationCue() override;

    virtual void StartCueInternal() override;
    virtual void TickInternal(double currentTime, double deltaTime, double clockTime) override;
    virtual void EndCueInternal() override;

private:
    vtkRenderer* _renderer;
    CameraInfo _startCamera;
    CameraInfo _endCamera;
};

#endif // CAMERAANIMATION_H
