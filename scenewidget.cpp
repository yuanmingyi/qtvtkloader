#include "scenewidget.h"
#include "timerutil.h"
#include "vtk_obj_reader.h"
#include <vtkSmartPointer.h>
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
        widget->pickedActorChanged(*static_cast<int*>(callData));
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
        this->m_renderer->RemoveActor(*it);
    }
    this->GetInteractor()->Render();
}

void SceneWidget::RenderProps()
{
    auto actors= this->m_objImporter->GetActors();
    for (auto it = actors.begin(); it != actors.end(); it++) {
        this->m_renderer->AddActor(*it);
    }
    this->m_renderer->ResetCamera();
    m_tm.start();
    m_startrender = true;
    this->GetInteractor()->Render();
}
