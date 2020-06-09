#include "demointeractorstyle.h"

#include "vtkNew.h"
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
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkSphereSource.h>
#include "cameraanimationcue.h"
#include "vtksys/SystemTools.hxx"


vtkStandardNewMacro(DemoInteractorStyle);

DemoInteractorStyle::DemoInteractorStyle()
{
    pickEventHandler = nullptr;
    pickEventClientData = nullptr;
    _selectedActor = nullptr;
    _opacity = 1;
}

DemoInteractorStyle::~DemoInteractorStyle()
{
    pickEventHandler = nullptr;
}

void DemoInteractorStyle::Initialize()
{
    auto picker = this->Interactor->GetPicker();
    picker->AddObserver(vtkCommand::PickEvent, this, &DemoInteractorStyle::PickEventCallback);

    _cameraCue->SetTimeModeToNormalized();
    _cameraCue->SetStartTime(0);
    _cameraCue->SetEndTime(1);
    _cameraCue->Renderer = this->DefaultRenderer;
    _cameraCue->RenWin = this->DefaultRenderer->GetRenderWindow();
}

void DemoInteractorStyle::PickEventCallback()
{
    auto picker = dynamic_cast<vtkPropPicker*>(this->Interactor->GetPicker());
    vtkActor* actor = picker->GetActor();
    if (actor)
    {
        std::cout << "pick actor: " << actor->GetProperty()->GetInformation() << std::endl;
        for (size_t i = 0; i < this->_actors.size(); i++)
        {
            if (this->_actors[i] == actor)
            {
                std::cout << "actor id: " << i << std::endl;
                this->PickActor(static_cast<int>(i));
                break;
            }
        }
    }
}

// implements vtkInteractorStyleTrackballCamera
void DemoInteractorStyle::OnChar()
{
    vtkRenderWindowInteractor *rwi = this->Interactor;
    std::string key = rwi->GetKeySym();

    if (key == "3")
    {
        // do not invoke the default event handler
        return;
    }

    vtkInteractorStyleTrackballCamera::OnChar();
}

void DemoInteractorStyle::OnKeyPress()
{
    // Get the keypress
    vtkRenderWindowInteractor *rwi = this->Interactor;
    std::string key = vtksys::SystemTools::LowerCase(rwi->GetKeySym());

    // Output the key that was pressed
    std::cout << "Pressed " << key << std::endl;

    try
    {
        this->PickActor(std::stoi(key));
    }
    catch(std::exception)
    {
        if (key == "escape")
        {
            // unselect the actor
            std::cout << "unselect" << std::endl;
            this->PickActor(-1);
        }
        else if (key == "h")
        {
            // hide actor
            std::cout << "hide the selected actor" << std::endl;
            this->SetSelectedVisible(false);
        }
        else if (key == "v")
        {
            // show actor
            std::cout << "show the selected actor" << std::endl;
            this->SetSelectedVisible();
        }
        else if (key == "m")
        {
            std::cout << "hide the unselected actors" << std::endl;
            this->SetUnselectedVisible(false);
        }
        else if (key == "n")
        {
            std::cout << "show the unselected actors" << std::endl;
            this->SetUnselectedVisible();
        }
        else if (key == "space")
        {
            // store camera
            this->ReserveCamera();
        }
        else if (key == "c")
        {
            // recover camera
            this->RestoreCamera();
        }
        else if (key == "o")
        {
            this->_opacity -= 0.2;
            if (this->_opacity < 0)
            {
                this->_opacity = 1;
            }
            std::cout << "opacity: " << this->_opacity << std::endl;
            this->SetUnselectedOpacity(this->_opacity);
        }
    }

    vtkInteractorStyleTrackballCamera::OnKeyPress();
}

void DemoInteractorStyle::ReserveCamera()
{
    this->_cameraCue->ReserverCamera();
}

void DemoInteractorStyle::RestoreCamera()
{
    this->StartCameraAnimation();
}

void DemoInteractorStyle::StartCameraAnimation()
{
    if (this->_cameraCue->HasCameraReserved())
    {
        vtkNew<vtkAnimationScene> scene;
        scene->RemoveAllCues();
        scene->AddCue(this->_cameraCue);
        scene->SetModeToRealTime();
        //scene->SetModeToSequence();
        scene->SetLoop(false);
        scene->SetFrameRate(5);
        scene->SetStartTime(0);
        scene->SetEndTime(1);
        scene->Play();
        scene->Stop();
    }
}

void DemoInteractorStyle::SetSelectedOpacity(double opacity)
{
    if (this->_selectedActor)
    {
        this->_selectedActor->GetProperty()->SetOpacity(opacity);
        this->GetCurrentRenderer()->GetRenderWindow()->Render();
    }
}

void DemoInteractorStyle::SetUnselectedOpacity(double opacity)
{
    for (size_t i = 0; i < this->_actors.size(); i++)
    {
        auto actor = this->_actors[i];
        if (actor != this->_selectedActor) {
            actor->GetProperty()->SetOpacity(opacity);
        }
    }
    this->GetCurrentRenderer()->GetRenderWindow()->Render();
}

void DemoInteractorStyle::PickActor(int actorIndex)
{
    vtkActor* actor = nullptr;
    if (actorIndex >= 0 && static_cast<size_t>(actorIndex) < this->_actors.size()) {
        std::cout << "pick actor: " << actorIndex << std::endl;
        actor = this->_actors[static_cast<size_t>(actorIndex)];
    }
    this->PickActor(actor);

    if (this->pickEventHandler) {
        this->pickEventHandler(this, PICK_EVENT, this->pickEventClientData, &actorIndex);
    }
}

void DemoInteractorStyle::PickActor(vtkActor* actor)
{
    if (this->_selectedActor == actor) {
        return;
    }

    if (this->_selectedActor)
    {
        std::cout << "restore the property of last selected actor" << std::endl;
        this->_selectedActor->GetProperty()->DeepCopy(this->_selectedActorProperty);
    }

    // unselect the previous picked actor
    this->_selectedActor = actor;
    if (this->_selectedActor && this->_selectedActor->GetProperty()) {
        std::cout << "set color for picked actor" << std::endl;
        //std::cout << "actor of mtl: " << this->_selectedActor->GetProperty()->GetMaterialName() << std::endl;
        this->_selectedActorProperty->DeepCopy(this->_selectedActor->GetProperty());
        this->_selectedActor->GetProperty()->SetColor(1.0, 1.0, 0);   // yellow
        this->_selectedActor->GetProperty()->SetDiffuse(0.5);
        this->_selectedActor->GetProperty()->SetAmbient(0.3);
        this->_selectedActor->GetProperty()->SetSpecular(0.1);
        this->_selectedActor->GetProperty()->SetSpecularColor(1.0, 0.0, 0.0);
        this->GetCurrentRenderer()->GetRenderWindow()->Render();
    }
}

void DemoInteractorStyle::SetSelectedVisible(bool visible)
{
    if (this->_selectedActor)
    {
        this->_selectedActor->SetVisibility(visible);
        this->GetCurrentRenderer()->GetRenderWindow()->Render();
    }
}

void DemoInteractorStyle::SetUnselectedVisible(bool visible)
{
    for (auto it = this->_actors.begin(); it != this->_actors.end(); it++)
    {
        if (*it != this->_selectedActor)
        {
            (*it)->SetVisibility(visible);
        }
    }
    this->GetCurrentRenderer()->GetRenderWindow()->Render();
}
