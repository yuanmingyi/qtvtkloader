#include "scenewidget.h"
#include "timerutil.h"
#include "vtk_obj_reader.h"
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkGenericOpenGLRenderWindow.h>


void SceneWidget::OnPickEvent(void* vtkNotUsed(caller), unsigned long eventId, void* clientData, void* callData)
{
    if (eventId == DemoInteractorStyle::PICK_EVENT) {
        auto widget = static_cast<SceneWidget*>(clientData);
        auto name = static_cast<std::string*>(callData);
        widget->pickedModuleChanged(*name);
    }
}

SceneWidget::SceneWidget(QWidget *parent)
  : QVTKOpenGLNativeWidget(parent)
{
//    vtkNew<vtkCallbackCommand> callback;
//    callback->SetCallback(SceneWidget::CompleteRender);
//    callback->SetClientData(this);
//    m_renderer->AddObserver(vtkCommand::EndEvent, callback);
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

    _dongfeng = new DongfengVis(m_renderer);

    resize(800, 600);
}

SceneWidget::~SceneWidget()
{
    m_demoStyle->pickEventHandler = nullptr;
    m_demoStyle->pickEventClientData = nullptr;
    delete _dongfeng;
}

void SceneWidget::ImportObj(const QString& filename)
{
    _dongfeng->ImportObj(filename.toStdString());
}

void SceneWidget::SetLightIntensity(double intensity)
{
    std::cout << "change intensity: " << intensity << std::endl;
    m_light->SetIntensity(intensity);
    GetInteractor()->Render();
}

// Slots
void SceneWidget::zoomToExtent()
{
    std::cout << "zoom to extent" << std::endl;
    this->m_renderer->ResetCamera();
    GetInteractor()->Render();
}

void SceneWidget::PickModule(const std::string &moduleName)
{
    if (moduleName != DongfengVis::None) {
        double color[3] = { 1, 1, 0 };
        _dongfeng->HighlightOn(moduleName, new DongfengVis::HighlightArguments(color));
    } else {
        _dongfeng->HighlightOff(DongfengVis::All);
    }
}
