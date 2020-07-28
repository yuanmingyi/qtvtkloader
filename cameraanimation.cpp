#include "cameraanimation.h"
#include <vtkCamera.h>
#include <string>

void Print3d(std::string hint, double* value)
{
    std::cout << hint << "(" << value[0] << ", " << value[1] << ", " << value[2] << ")" << std::endl;
}

double* CalcNext(const double* start, const double* target, double ratio, double* next)
{
    vtkMath::Subtract(target, start, next);
    vtkMath::MultiplyScalar(next, ratio);
    vtkMath::Add(start, next, next);
    return next;
}

CameraAnimation::CameraAnimation()
{
    _sceneObserver = vtkSmartPointer<AnimationSceneObserver>::New();
    AddObserver(vtkCommand::AnimationCueTickEvent, _sceneObserver);
    AddObserver(vtkCommand::EndAnimationCueEvent, _sceneObserver);
}

void CameraAnimation::Play(std::function<void()> renderMethod, vtkRenderer* renderer, const CameraInfo& start, const CameraInfo& end, double time)
{
    _sceneObserver->SetRenderMethod(renderMethod);
    vtkNew<CameraAnimationCue> cue;
    cue->Setup(renderer, start, end);
    AddCue(cue);
    //SetModeToSequence();
    SetModeToRealTime();
    SetLoop(false);
    SetFrameRate(5);
    SetStartTime(0);
    SetEndTime(time);
    vtkAnimationScene::Play();
    Stop();
}

CameraAnimationCue::CameraAnimationCue()
{
    _renderer = nullptr;
    SetStartTime(0);
    SetEndTime(1);
    SetTimeModeToNormalized();
}

CameraAnimationCue::~CameraAnimationCue()
{
    if (_renderer) {
        _renderer->UnRegister(this);
        _renderer = nullptr;
    }
}

void CameraAnimationCue::Setup(vtkRenderer* renderer, const CameraInfo &startCamera, const CameraInfo &endCamera)
{
    _startCamera = startCamera;
    _endCamera = endCamera;
    _renderer = renderer;
    _renderer->Register(this);
}

void CameraAnimationCue::StartCueInternal()
{
    std::cout << "start to move camera" << std::endl;
    std::cout << "Start view angle: " << _startCamera.ViewAngle << std::endl;
    vtkAnimationCue::StartCueInternal();
}

void CameraAnimationCue::TickInternal(double currentTime, double deltaTime, double clockTime)
{
    double delta[3];
    auto camera = _renderer->GetActiveCamera();
    camera->SetPosition(CalcNext(_startCamera.Position, _endCamera.Position, currentTime, delta));
    camera->SetFocalPoint(CalcNext(_startCamera.FocalPoint, _endCamera.FocalPoint, currentTime, delta));
    camera->SetViewUp(CalcNext(_startCamera.ViewUp, _endCamera.ViewUp, currentTime, delta));
    // camera should be towards the target
    auto dist = _endCamera.ViewAngle - _startCamera.ViewAngle;
    if (dist > 180)
    {
        dist -= -360;
    }
    else if (dist < -180)
    {
        dist += 360;
    }
    camera->SetViewAngle(_startCamera.ViewAngle + dist * currentTime);
    _renderer->ResetCameraClippingRange();
    Print3d("[TickInternal()] camera position: ", camera->GetPosition());

    vtkAnimationCue::TickInternal(currentTime, deltaTime, clockTime);
}

void CameraAnimationCue::EndCueInternal()
{
    vtkCamera* camera = _renderer->GetActiveCamera();
    camera->SetPosition(_endCamera.Position);
    camera->SetFocalPoint(_endCamera.FocalPoint);
    camera->SetViewUp(_endCamera.ViewUp);
    camera->SetViewAngle(_endCamera.ViewAngle);
    _renderer->ResetCameraClippingRange();

    vtkAnimationCue::EndCueInternal();
}
