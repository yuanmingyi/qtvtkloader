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
    double Roll;
};

class CameraAnimation : public vtkAnimationScene
{
public:
    static CameraAnimation* New()
    {
        return new CameraAnimation();
    }

    void Play(std::function<void()> renderMethod, vtkRenderer* renderer, const CameraInfo& start, const CameraInfo& end, double time = 1);

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
