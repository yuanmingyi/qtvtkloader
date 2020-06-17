#include "scenewidget.h"
#include "timerutil.h"
#include "vtk_obj_reader.h"
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QCoreApplication>
#include <QDebug>

SceneWidget::SceneWidget(QWidget *parent)
  : QVTKOpenGLNativeWidget(parent)
{
    _isTiming = false;
    _isFullImported = false;
    _renderer->SetBackground(.3, .3, .3);

    _light->SetLightTypeToHeadlight();
    _light->PositionalOff();
    _light->SetColor(1, 1, 1);
    _light->SetIntensity(1);
    _renderer->AddLight(_light);

    _axes->SetVisibility(false);
    _axes->SetTotalLength(50, 50, 50);
    _axes->SetShaftTypeToLine();
    _axes->GetXAxisCaptionActor2D()->SetWidth(0.03);
    _axes->GetYAxisCaptionActor2D()->SetWidth(0.03);
    _axes->GetZAxisCaptionActor2D()->SetWidth(0.03);
    _axes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->BoldOff();
    _axes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->BoldOff();
    _axes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->BoldOff();
    _renderer->AddActor(_axes);

    vtkNew<vtkGenericOpenGLRenderWindow> window;
    auto iren = GetInteractor();
    iren->SetInteractorStyle(vtkInteractorStyleTrackballCamera::New());
    iren->SetRenderWindow(window.Get());
    SetRenderWindow(window.Get());
    GetRenderWindow()->AddRenderer(_renderer);
    _dongfeng = new DongfengVis();
    _dongfeng->SetRenderMethod([this]() {
        this->GetInteractor()->Render();
        QCoreApplication::processEvents();
    });

    resize(800, 600);
}

SceneWidget::~SceneWidget()
{
    delete _dongfeng;
}

void SceneWidget::ImportObj(const QString& filename, bool loadTexture)
{
    StartTimer();
    _dongfeng->ImportObj(filename.toStdString(), _renderer, loadTexture);
    EndTimer("Impoort time:");
    _renderer->ResetCamera();
    qDebug() << "render window: " << GetRenderWindow() << endl;
    GetInteractor()->Render();
}

void SceneWidget::SetOpacity(double opacity)
{
    std::cout << "set opacity: " << opacity << std::endl;
    _dongfeng->SetOpacity(opacity);
    GetInteractor()->Render();
}

void SceneWidget::SetLightIntensity(double intensity)
{
    std::cout << "change intensity: " << intensity << std::endl;
    _light->SetIntensity(intensity);
    qDebug() << "render window: " << GetRenderWindow() << endl;
    GetInteractor()->Render();
}

void SceneWidget::PickModule(const std::string &moduleName)
{
    double color[3] = { 1, 1, 0 };
    _dongfeng->Highlight(moduleName, DongfengVis::HighlightArguments(color));
    qDebug() << "render window: " << GetRenderWindow() << endl;
    GetInteractor()->Render();
}

// Slots
void SceneWidget::zoomToExtent()
{
    std::cout << "zoom to extent" << std::endl;
    this->_renderer->ResetCamera();
    qDebug() << "render window: " << GetRenderWindow() << endl;
    GetInteractor()->Render();
}

// private methods
void SceneWidget::StartTimer()
{
    if (!_isTiming) {
        qDebug() << "start timing" << endl;
        _tm.start();
        _isTiming = true;
    } else {
        qDebug() << "start failed. Already in timing" << endl;
    }
}

void SceneWidget::EndTimer(const QString& context)
{
    if (_isTiming) {
        qDebug() << "end timing" << endl;
        _isTiming = false;
        _tm.end();
        qDebug() << context << _tm.msec() << "ms" << endl;
    } else {
        qDebug() << context << " end timer failed. Not in timing" << endl;
    }
}
