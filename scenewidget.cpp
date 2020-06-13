#include "scenewidget.h"
#include "timerutil.h"
#include "vtk_obj_reader.h"
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QDebug>

void SceneWidget::RendererEventHandler(vtkObject* caller, unsigned long eventId, void* vtkNotUsed(clientData), void* vtkNotUsed(callData))
{
    auto widget = dynamic_cast<SceneWidget*>(caller);
    if (eventId == vtkCommand::StartEvent) {
        qDebug() << "renderer start event" << endl;
        widget->m_tm.start();
    } else if (eventId == vtkCommand::EndEvent) {
        qDebug() << "renderer end event" << endl;
        widget->m_tm.end();
        qDebug() << "Render time: " << widget->m_tm.msec() << "ms" << endl;
    }
}

SceneWidget::SceneWidget(QWidget *parent)
  : QVTKOpenGLNativeWidget(parent)
{
    vtkNew<vtkCallbackCommand> rendererEventHandler;
    rendererEventHandler->SetCallback(SceneWidget::RendererEventHandler);
    rendererEventHandler->SetClientData(this);
    m_renderer->AddObserver(vtkCommand::EndEvent, rendererEventHandler);
    m_renderer->AddObserver(vtkCommand::StartEvent, rendererEventHandler);

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
    iren->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
    iren->SetRenderWindow(window.Get());
    SetRenderWindow(window.Get());
    GetRenderWindow()->AddRenderer(m_renderer);
    _dongfeng = new DongfengVis(m_renderer);

    resize(800, 600);
}

SceneWidget::~SceneWidget()
{
    delete _dongfeng;
}

void SceneWidget::ImportObj(const QString& filename)
{
    m_tm.start();
    _dongfeng->ImportObj(filename.toStdString());
    m_tm.end();
    qDebug() << "Import time: " << m_tm.msec() << "ms" << endl;
    this->m_renderer->ResetCamera();
    GetInteractor()->Render();
}

void SceneWidget::SetLightIntensity(double intensity)
{
    std::cout << "change intensity: " << intensity << std::endl;
    m_light->SetIntensity(intensity);
    GetInteractor()->Render();
}

void SceneWidget::PickModule(const std::string &moduleName)
{
    double color[3] = { 1, 1, 0 };
    _dongfeng->Highlight(moduleName, DongfengVis::HighlightArguments(color));
    GetInteractor()->Render();
}

// Slots
void SceneWidget::zoomToExtent()
{
    std::cout << "zoom to extent" << std::endl;
    this->m_renderer->ResetCamera();
    GetInteractor()->Render();
}
