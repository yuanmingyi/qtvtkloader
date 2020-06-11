#include "scenewidget.h"
#include "timerutil.h"
#include "vtk_obj_reader.h"
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkGenericOpenGLRenderWindow.h>


// static
void SceneWidget::CompleteRender(vtkObject* vtkNotUsed(caller), long unsigned int eventId, void* clientData, void* vtkNotUsed(callData))
{
    if (eventId == vtkCommand::EndEvent) {
        auto obj = static_cast<SceneWidget*>(clientData);
        obj->CompleteRender();
    }
}

void SceneWidget::OnPickEvent(void* vtkNotUsed(caller), unsigned long eventId, void* clientData, void* callData)
{
    if (eventId == DemoInteractorStyle::PICK_EVENT) {
        auto widget = static_cast<SceneWidget*>(clientData);
        widget->UpdatePickedActor(*static_cast<int*>(callData));
    }
}

SceneWidget::SceneWidget(QWidget *parent)
  : QVTKOpenGLNativeWidget(parent)
{
    vtkNew<vtkCallbackCommand> callback;
    callback->SetCallback(SceneWidget::CompleteRender);
    callback->SetClientData(this);
    m_renderer->AddObserver(vtkCommand::EndEvent, callback);
    m_renderer->SetBackground(.3, .3, .3);

    m_light->SetLightTypeToHeadlight();
    m_light->PositionalOff();
    m_light->SetColor(1, 1, 1);
    m_light->SetIntensity(1);
    m_renderer->AddLight(m_light);

    m_axes->SetVisibility(false);
    m_axes->SetTotalLength(50, 50, 50);
    m_axes->SetShaftTypeToLine();
    m_axes->GetXAxisCaptionActor2D()->SetWidth(0.03);
    m_axes->GetYAxisCaptionActor2D()->SetWidth(0.03);
    m_axes->GetZAxisCaptionActor2D()->SetWidth(0.03);
    m_axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->BoldOff();
    m_axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->BoldOff();
    m_axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->BoldOff();
    m_renderer->AddActor(m_axes);

    vtkNew<vtkGenericOpenGLRenderWindow> window;
    auto iren = GetInteractor();
    iren->SetInteractorStyle(m_demoStyle);
    iren->SetRenderWindow(window.Get());
    SetRenderWindow(window.Get());
    GetRenderWindow()->AddRenderer(m_renderer);
    m_demoStyle->SetDefaultRenderer(m_renderer);
    m_demoStyle->Initialize();
    m_demoStyle->pickEventHandler = SceneWidget::OnPickEvent;
    m_demoStyle->pickEventClientData = this;
    m_objImporter = new ObjImporter();

    resize(800, 600);
}

SceneWidget::~SceneWidget()
{
    m_demoStyle->pickEventHandler = nullptr;
    m_demoStyle->pickEventClientData = nullptr;
    delete m_objImporter;
}

void SceneWidget::ImportObj(const QString& filename)
{
    // remmove old actors
    ClearProps();
    m_tm.start();
    m_objImporter->Import(filename.toStdString().data());
    m_demoStyle->SetActors(m_objImporter->GetActors());
    m_tm.end();
    std::cout << "Import time: " << m_tm.msec() << "ms" << std::endl;
    RenderProps();
}

void SceneWidget::SetLightIntensity(double intensity)
{
    std::cout << "change intensity: " << intensity << std::endl;
    m_light->SetIntensity(intensity);
    GetInteractor()->Render();
}

double* SceneWidget::GetActorCenterBounds(int actorIndex, double* bounds)
{
    auto actors = m_objImporter->GetActors();
    if (actorIndex >= 0 && static_cast<size_t>(actorIndex) < actors.size()) {
        auto actor = actors[static_cast<size_t>(actorIndex)];
        if (bounds != nullptr) {
            actor->GetBounds(bounds);
        }
        return actor->GetCenter();
    }
    return nullptr;
}

// Slots
void SceneWidget::zoomToExtent()
{
    std::cout << "zoom to extent" << std::endl;
    this->m_renderer->ResetCamera();
    GetInteractor()->Render();
}

void SceneWidget::PickActor(int index)
{
    m_demoStyle->PickActor(index);
}

void SceneWidget::CompleteRender()
{
    if (m_startrender) {
        m_startrender = false;
        m_tm.end();
        std::cout << "Render time: " << m_tm.msec() << "ms" << std::endl;
    }
}

void SceneWidget::ClearProps()
{
    auto actors = this->m_objImporter->GetActors();
    for (auto it = actors.begin(); it != actors.end(); it++) {
        this->m_renderer->RemoveViewProp(*it);
    }
//    auto rootObj = this->m_objImporter->GetRootObject();
//    this->m_renderer->RemoveViewProp(rootObj);
    this->GetInteractor()->Render();
}

void SceneWidget::RenderProps()
{
    auto actors= this->m_objImporter->GetActors();
    for (auto it = actors.begin(); it != actors.end(); it++) {
        this->m_renderer->AddViewProp(*it);
    }
//    auto rootObj = m_objImporter->GetRootObject();
//    this->m_renderer->AddViewProp(rootObj);
    this->m_renderer->ResetCamera();
    m_tm.start();
    m_startrender = true;
    this->GetInteractor()->Render();
}

void SceneWidget::UpdatePickedActor(int actorIndex)
{
    vtkNew<vtkTransform> tf;
    double *center = GetActorCenterBounds(actorIndex);
    if (center) {
        std::cout << "actor center: (" << center[0] << ", " << center[1] << ", " << center[2] << ")" << std::endl;
        tf->Translate(center);
    } else {
        tf->Translate(0, 0, 0);
    }
    this->m_axes->SetUserTransform(tf);
    this->pickedActorChanged(actorIndex);
}
