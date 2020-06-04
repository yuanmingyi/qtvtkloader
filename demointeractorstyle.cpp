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

DemoInteractorStyle::DemoInteractorStyle()
{
    _selectedActor = nullptr;
    _sphereSource->SetRadius(10);
    _sphereMapper->SetInputConnection(_sphereSource->GetOutputPort());
    _focusedStyle = FocusedStyle::Bound;
    _opacity = 1;
}

static std::vector<vtkActor*> actorArray;

void DemoInteractorStyle::Initialize()
{
    this->DefaultRenderer->AddActor(_focusedActor);
    this->DefaultRenderer->AddActor(_axes);

    auto picker = this->Interactor->GetPicker();
    picker->AddObserver(vtkCommand::PickEvent, this, &DemoInteractorStyle::PickEventCallback);


    _axes->SetTotalLength(2, 2, 2);
    _axes->SetShaftType(0);
    _axes->SetCylinderRadius(0.02);
    _axes->GetXAxisCaptionActor2D()->SetWidth(0.03);
    _axes->GetYAxisCaptionActor2D()->SetWidth(0.03);
    _axes->GetZAxisCaptionActor2D()->SetWidth(0.03);

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
        for (size_t i = 0; i < actorArray.size(); i++)
        {
            if (actorArray[i] == actor)
            {
                std::cout << "actor id: " << i << std::endl;
                break;
            }
        }
        PickActor(actor);
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
        unsigned int idx = static_cast<unsigned int>(std::stoul(key));
        if (idx < actorArray.size())
        {
            std::cout << "pick actor: " << idx << std::endl;
            PickActor(actorArray[idx]);
        }
    }
    catch(std::exception)
    {
        if (key == "escape")
        {
            // unselect the actor
            std::cout << "unselect" << std::endl;
            PickActor(nullptr);
        }
        else if (key == "h")
        {
            // hide actor
            std::cout << "hide the selected actor" << std::endl;
            SetSelectedVisible(false);
        }
        else if (key == "v")
        {
            // show actor
            std::cout << "show the selected actor" << std::endl;
            SetSelectedVisible();
        }
        else if (key == "m")
        {
            std::cout << "hide the unselected actors" << std::endl;
            SetUnselectedVisible(false);
        }
        else if (key == "n")
        {
            std::cout << "show the unselected actors" << std::endl;
            SetUnselectedVisible();
        }
        else if (key == "z")
        {
            _focusedStyle = static_cast<FocusedStyle>((_focusedStyle + 1) % 3);
            std::cout << "Focused Style: " << _focusedStyle << std::endl;
            UpdateFocusActor();
        }
        else if (key == "a")
        {
            // select actor from command line
            std::cout << "Input actor id: ";
            size_t actorId = 0;
            std::cin >> actorId;
            if (actorId < actorArray.size())
            {
                std::cout << "pick actor: " << actorId << std::endl;
                PickActor(actorArray[actorId]);
            }
            else
            {
                std::cout << "invalid actor id" << std::endl;
            }
        }
        else if (key == "space")
        {
            // store camera
            ReserveCamera();
        }
        else if (key == "c")
        {
            // recover camera
            RestoreCamera();
        }
        else if (key == "o")
        {
            _opacity -= 0.2;
            if (_opacity < 0)
            {
                _opacity = 1;
            }
            std::cout << "opacity: " << _opacity << std::endl;
            for (size_t i = 0; i < actorArray.size(); i++)
            {
                auto actor = actorArray[i];
                actor->GetProperty()->SetOpacity(_opacity);
            }
            this->GetCurrentRenderer()->GetRenderWindow()->Render();
        }
    }

    vtkInteractorStyleTrackballCamera::OnKeyPress();
}

void DemoInteractorStyle::ReserveCamera()
{
    _cameraCue->ReserverCamera();
}

void DemoInteractorStyle::RestoreCamera()
{
    StartCameraAnimation();
}

void DemoInteractorStyle::StartCameraAnimation()
{
    if (_cameraCue->HasCameraReserved())
    {
        vtkNew<vtkAnimationScene> scene;
        scene->RemoveAllCues();
        scene->AddCue(_cameraCue);
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

void DemoInteractorStyle::PickActor(vtkActor* actor)
{
    if (_selectedActor)
    {
        std::cout << "restore the property of last selected actor" << std::endl;
        _selectedActor->GetProperty()->DeepCopy(_selectedActorProperty);
    }

    // unselect the previous picked actor
    _selectedActor = actor;
    if (_selectedActor)
    {
        std::cout << "actor of mtl: " << _selectedActor->GetProperty()->GetMaterialName() << std::endl;
        _selectedActorProperty->DeepCopy(_selectedActor->GetProperty());
        _axes->SetPosition(_selectedActor->GetOrigin());
        _axes->SetVisibility(true);
    }
    else
    {
        _axes->SetVisibility(false);
    }

    UpdateFocusActor();
}

void DemoInteractorStyle::SetSelectedVisible(bool visible)
{
    if (_selectedActor)
    {
        _selectedActor->SetVisibility(visible);
        this->GetCurrentRenderer()->GetRenderWindow()->Render();
    }
}

void DemoInteractorStyle::SetUnselectedVisible(bool visible)
{
    for (auto it = actorArray.begin(); it != actorArray.end(); it++)
    {
        if (*it != _selectedActor)
        {
            (*it)->SetVisibility(visible);
        }
    }
    this->GetCurrentRenderer()->GetRenderWindow()->Render();
}

void DemoInteractorStyle::UpdateFocusActor()
{
    if (_selectedActor)
    {
        if (_focusedStyle == FocusedStyle::Bound)
        {
            SetupBoundFocusedActor();
        }
        else if (_focusedStyle == FocusedStyle::Center)
        {
            SetupPointFocusedActor(_selectedActor->GetCenter());
        }
        else if (_focusedStyle == FocusedStyle::Origin)
        {
            SetupPointFocusedActor(_selectedActor->GetOrigin());
        }
        _focusedActor->SetVisibility(true);
    }
    else
    {
        _focusedActor->SetVisibility(false);
    }

    this->GetCurrentRenderer()->GetRenderWindow()->Render();
}

void DemoInteractorStyle::SetupPointFocusedActor(double *pt)
{
    _focusedActor->SetMapper(_sphereMapper);
    _focusedActor->SetPosition(pt);
    _focusedActor->GetProperty()->SetColor(1.0, 0, 0);
}

void DemoInteractorStyle::SetupBoundFocusedActor()
{
    _focusedActor->SetMapper(_selectedActor->GetMapper());
    _focusedActor->SetPosition(_selectedActor->GetPosition());
    _focusedActor->SetOrientation(_selectedActor->GetOrientation());
    _focusedActor->SetOrigin(_selectedActor->GetOrigin());
    _focusedActor->GetProperty()->EdgeVisibilityOn();
    _focusedActor->GetProperty()->SetColor(1, 1, 0);
}

