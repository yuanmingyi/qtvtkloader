#include "cameraanimationcue.h"

#include <vtkMath.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkAnimationCue.h>

void Print3d(std::string hint, double* value)
{
    std::cout << hint << "(" << value[0] << ", " << value[1] << ", " << value[2] << ")" << std::endl;
}

double* CalcNext(const double* current, const double* target, double ratio, double* next)
{
    vtkMath::Subtract(target, current, next);
    vtkMath::MultiplyScalar(next, ratio);
    vtkMath::Add(current, next, next);
    return next;
}

CameraAnimationCue::CameraAnimationCue() : vtkAnimationCue()
{
    Renderer = nullptr;
    RenWin = nullptr;
    _cameraReserved = false;
}

void CameraAnimationCue::ReserverCamera()
{
    vtkCamera* camera = Renderer->GetActiveCamera();
    camera->GetPosition(_cameraPosition);
    camera->GetFocalPoint(_cameraFocalPoint);
    _cameraRoll = camera->GetRoll();
    std::cout << "Camera roll angle: " << _cameraRoll << std::endl;
    std::cout << "Camera view up dot camera direction: " << vtkMath::Dot(camera->GetViewUp(), camera->GetDirectionOfProjection()) << std::endl;
    _cameraReserved = true;
}

bool CameraAnimationCue::HasCameraReserved()
{
    return _cameraReserved;
}

void CameraAnimationCue::StartCueInternal()
{
    std::cout << "start to move camera" << std::endl;
    vtkCamera* camera = Renderer->GetActiveCamera();
    camera->GetPosition(_startPosition);
    camera->GetFocalPoint(_startFocalPoint);
    _startRoll = camera->GetRoll();
    std::cout << "Start roll angle: " << _startRoll << std::endl;
    vtkAnimationCue::StartCueInternal();
}

void CameraAnimationCue::TickInternal(double currentTime, double deltaTime, double clockTime)
{
    auto camera = Renderer->GetActiveCamera();
    double delta[3];
    if (this->GetTimeMode() == TIMEMODE_RELATIVE)
    {
        std::cout << "Cue is relative time mode, convert to normalized mode first" << std::endl;
        // make the time normalized
        currentTime = currentTime / (this->GetEndTime() - this->GetStartTime());
    }

    camera->SetPosition(CalcNext(_startPosition, _cameraPosition, currentTime, delta));
    camera->SetFocalPoint(CalcNext(_startFocalPoint, _cameraFocalPoint, currentTime, delta));
    // camera should be towards the target
    auto dist = _cameraRoll - _startRoll;
    if (dist > 180)
    {
        dist -= -360;
    }
    else if (dist < -180)
    {
        dist += 360;
    }
    camera->SetRoll(_startRoll + dist * currentTime);
    Renderer->ResetCameraClippingRange();

    if (RenWin)
    {
        RenWin->Render();
    }

    vtkAnimationCue::TickInternal(currentTime, deltaTime, clockTime);
}

void CameraAnimationCue::EndCueInternal()
{
    vtkCamera* camera = Renderer->GetActiveCamera();
    camera->SetPosition(_cameraPosition);
    camera->SetFocalPoint(_cameraFocalPoint);
    camera->SetRoll(_cameraRoll);
    Renderer->ResetCameraClippingRange();
    std::cout << "Camera new roll angle: " << camera->GetRoll() << std::endl;
    std::cout << "Camera view up dot camera direction: " << vtkMath::Dot(camera->GetViewUp(), camera->GetDirectionOfProjection()) << std::endl;

    if (RenWin)
    {
        RenWin->Render();
    }

    vtkAnimationCue::EndCueInternal();
}
