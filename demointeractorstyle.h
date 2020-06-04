#ifndef DEMOINTERACTORSTYLE_H
#define DEMOINTERACTORSTYLE_H

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkNamedColors.h>
#include <vtkOBJImporter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkInformation.h>
#include <vtkTexture.h>
#include <vtkLight.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkProperty2D.h>
#include <vtkPropPicker.h>
#include <vtkLODActor.h>
#include <vtkSliderWidget.h>
#include <vtkSliderRepresentation2D.h>
#include <vtkTextActor.h>
#include <vtkTextProperty.h>
#include <vtkAxesActor.h>
#include <vtkCaptionActor2D.h>
#include <vtkAnimationCue.h>
#include <vtkAnimationScene.h>
#include <vtkAssembly.h>
#include <vtkSphereSource.h>
#include "cameraanimationcue.h"

class DemoInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    enum FocusedStyle
    {
        Bound,
        Center,
        Origin
    };

    static DemoInteractorStyle* New();
    vtkTypeMacro(DemoInteractorStyle, vtkInteractorStyleTrackballCamera)

    DemoInteractorStyle();

    void Initialize();
    void SliderChangeEventCallback();
    void PickEventCallback();

    // implements vtkInteractorStyleTrackballCamera
    virtual void OnChar() override;

    virtual void OnKeyPress() override;

private:
    vtkActor* _selectedActor;
    vtkNew<vtkLODActor> _focusedActor;
    vtkNew<vtkSphereSource> _sphereSource;
    vtkNew<vtkPolyDataMapper> _sphereMapper;
    vtkNew<vtkAxesActor> _axes;
    vtkNew<vtkProperty> _selectedActorProperty;
    vtkNew<CameraAnimationCue> _cameraCue;
    double _opacity;

    FocusedStyle _focusedStyle;

    void ReserveCamera();
    void RestoreCamera();
    void StartCameraAnimation();
    void PickActor(vtkActor* actor);
    void SetSelectedVisible(bool visible = true);
    void SetUnselectedVisible(bool visible = true);
    void UpdateFocusActor();
    void SetupPointFocusedActor(double *pt);
    void SetupBoundFocusedActor();
};

vtkStandardNewMacro(DemoInteractorStyle);

#endif // DEMOINTERACTORSTYLE_H
