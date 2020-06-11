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
    static const unsigned long PICK_EVENT = 1;

    typedef void (*EventHandler)(void* caller, unsigned long eid, void* clientData, void* calldata);

    static DemoInteractorStyle* New();
    vtkTypeMacro(DemoInteractorStyle, vtkInteractorStyleTrackballCamera)

    DemoInteractorStyle();
    ~DemoInteractorStyle() override;

    void SetActors(const std::vector<vtkActor*>& actors) { if (this->_actors != actors) { this->_actors = actors; } }
    void Initialize();
    void PickEventCallback();
    void PickActor(int actorIndex);
    void SetSelectedVisible(bool visible = true);
    void SetUnselectedVisible(bool visible = true);
    void SetSelectedOpacity(double opacity);
    void SetUnselectedOpacity(double opacity);

    // implements vtkInteractorStyleTrackballCamera
    virtual void OnChar() override;

    virtual void OnKeyPress() override;

    EventHandler pickEventHandler;
    void* pickEventClientData;

private:
    std::vector<vtkActor*> _actors;
    vtkActor* _selectedActor;
    vtkNew<vtkProperty> _selectedActorProperty;
    vtkNew<vtkAxesActor> _axes;
    vtkSmartPointer<vtkTexture> _selectedActorTexture;
    vtkNew<CameraAnimationCue> _cameraCue;
    double _opacity;

    void ReserveCamera();
    void RestoreCamera();
    void StartCameraAnimation();
    void PickActor(vtkActor* actor);
};

#endif // DEMOINTERACTORSTYLE_H
