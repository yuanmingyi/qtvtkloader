#ifndef CAMERAANIMATIONCUE_H
#define CAMERAANIMATIONCUE_H

#include <vtkMath.h>
#include <vtkCamera.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkAnimationCue.h>

class CameraAnimationCue: public vtkAnimationCue
{
public:
    static CameraAnimationCue* New()
    {
        return new CameraAnimationCue();
    }

    void ReserverCamera();
    bool HasCameraReserved();
    vtkRenderer* Renderer;
    vtkRenderWindow* RenWin;

protected:
    CameraAnimationCue();
    virtual void StartCueInternal() override;
    virtual void TickInternal(double currentTime, double deltaTime, double clockTime) override;
    virtual void EndCueInternal() override;

private:
    double _cameraPosition[3];
    double _cameraFocalPoint[3];
    double _cameraRoll;
    double _startPosition[3];
    double _startFocalPoint[3];
    double _startRoll;
    bool _cameraReserved;
};

#endif // CAMERAANIMATIONCUE_H
