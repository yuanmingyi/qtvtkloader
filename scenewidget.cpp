#include "scenewidget.h"
#include "timerutil.h"
#include "util.h"
#include "vtk_obj_reader.h"
#include <vtkSmartPointer.h>
#include <vtkTransform.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkDepthSortPolyData.h>
#include <QCoreApplication>
#include <QDebug>

SceneWidget::SceneWidget(QWidget *parent)
  : QVTKOpenGLNativeWidget(parent)
{
    _isTiming = false;
    _isFullImported = false;

    _light->SetLightTypeToHeadlight();
    _light->PositionalOff();
    _light->SetColor(1, 1, 1);
    _light->SetIntensity(1);
    _renderer->AddLight(_light);

    _highlightColor[0] = 1;
    _highlightColor[1] = 1;
    _highlightColor[2] = 0;

    _backgroundColor[0] = 0.3;
    _backgroundColor[1] = 0.3;
    _backgroundColor[2] = 0.3;
    _renderer->SetBackground(_backgroundColor);

    _modelColor[0] = 0.7;
    _modelColor[1] = 0.7;
    _modelColor[2] = 0.7;

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
    _dongfeng->SetColor(_modelColor[0], _modelColor[1], _modelColor[2]);

    _maxPeels = 10;
    _occulusionRatio = 0.1;
    _useDepthPeeling = util::IsDepthPeelingSupported(GetRenderWindow(), _renderer, true);
    if (_useDepthPeeling) {
        // GPU
        std::cout << "*** DEPTH PEELING ***" << std::endl;
        // Setup GPU depth peeling with configured parameters
        util::SetupEnvironmentForDepthPeeling(GetRenderWindow(), _renderer, _maxPeels, _occulusionRatio);
    }

    resize(800, 600);
}

SceneWidget::~SceneWidget()
{
    delete _dongfeng;
}

void SceneWidget::ImportObj(const std::string& filename, bool loadTexture, bool isEnableDepthSorting)
{
    StartTimer();
    _dongfeng->ImportObj(filename, _renderer, loadTexture, !_useDepthPeeling && isEnableDepthSorting);
    EndTimer("Impoort time:");
    _renderer->ResetCamera();
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
    GetInteractor()->Render();
}

void SceneWidget::PickModule(const std::string& moduleName)
{
    _dongfeng->Highlight(moduleName, DongfengVis::HighlightArguments(_highlightColor));
    GetInteractor()->Render();
}

void SceneWidget::SetHighlightColor(const double* color)
{
    SetHighlightColor(color[0], color[1], color[2]);
}

void SceneWidget::SetHighlightColor(double r, double g, double b)
{
    _highlightColor[0] = r;
    _highlightColor[1] = g;
    _highlightColor[2] = b;
    auto modules = _dongfeng->GetModuleNames();
    for (auto it = modules.begin(); it != modules.end(); it++) {
        if (_dongfeng->IsModuleHighlightOn(*it)) {
            _dongfeng->HighlightOn(*it, DongfengVis::HighlightArguments(_highlightColor));
        }
    }
    GetInteractor()->Render();
}

void SceneWidget::SetModelColor(const double *color)
{
    SetModelColor(color[0], color[1], color[2]);
}

void SceneWidget::SetModelColor(double r, double g, double b)
{
    _modelColor[0] = r;
    _modelColor[1] = g;
    _modelColor[2] = b;
    _dongfeng->SetColor(r, g, b);
    GetInteractor()->Render();
}

void SceneWidget::SetBackgroundColor(const double *color)
{
    SetBackgroundColor(color[0], color[1], color[2]);
}

void SceneWidget::SetBackgroundColor(double r, double g, double b)
{
    _backgroundColor[0] = r;
    _backgroundColor[1] = g;
    _backgroundColor[2] = b;
    _renderer->SetBackground(r, g, b);
    GetInteractor()->Render();
}

void SceneWidget::AnimateHighlight(const std::string& moduleName)
{
    _dongfeng->AnimateHighlight(moduleName, DongfengVis::HighlightArguments(_highlightColor));
    GetInteractor()->Render();
}

// Slots
void SceneWidget::zoomToExtent()
{
    std::cout << "zoom to extent" << std::endl;
    this->_renderer->ResetCamera();
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

void SceneWidget::EndTimer(const std::string& context)
{
    if (_isTiming) {
        qDebug() << "end timing" << endl;
        _isTiming = false;
        _tm.end();
        qDebug() << context.data() << _tm.msec() << "ms" << endl;
    } else {
        qDebug() << context.data() << " end timer failed. Not in timing" << endl;
    }
}
