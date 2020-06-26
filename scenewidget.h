#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QObject>
#include "timerutil.h"
#include "dongfengvis.h"
#include "demointeractorstyle.h"
#include <QVTKOpenGLNativeWidget.h>

class SceneWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    static void RendererEventHandler(vtkObject* caller, unsigned long eventId, void* clientData, void* callData);

    explicit SceneWidget(QWidget *parent = nullptr);
    ~SceneWidget();

    void ImportObj(const QString& fileName, bool loadTexture = false, bool enableDepthSorting = true);
    const std::vector<std::string>& GetPickableItems() const{ return _dongfeng->GetModuleNames(); }

    bool IsImported() const { return _isFullImported; }

    void SetOpacity(double opacity);
    void PickModule(const std::string& moduleName);
    void SetLightIntensity(double intensity);
    void ShowAxes(bool show) { _axes->SetVisibility(show); }
    bool IsAxesShown() { return _axes->GetVisibility(); }
    void SetHighlightColor(const double* color);
    void SetHighlightColor(double r, double g, double b);
    void SetModelColor(const double* color);
    void SetModelColor(double r, double g, double b);
    void SetBackgroundColor(const double* color);
    void SetBackgroundColor(double r, double g, double b);
    const double* GetHighlightColor() const { return _highlightColor; }
    const double* GetModelColor() const { return _modelColor; }
    const double* GetBackgroundColor() const { return _backgroundColor; }

    void AnimateHighlight(const std::string& moduleName);
    void AnimateDaofu(bool open = true) { open ? _dongfeng->AnimateDaofu(0, 1) : _dongfeng->AnimateDaofu(1, 0); }
    void AnimateBiantianxian(bool open = true) { open ? _dongfeng->AnimateBiantianxian(0, 1) : _dongfeng->AnimateBiantianxian(1, 0); }
    void AnimateShengjianggan(bool open = true) { open ? _dongfeng->AnimateShengjianggan(0, 1) : _dongfeng->AnimateShengjianggan(1, 0); }
    void AnimateZuobanHorizontal(bool open = true) { open ? _dongfeng->AnimateZuobanHorizontal(0, 1) : _dongfeng->AnimateZuobanHorizontal(1, 0); }
    void AnimateZuobanVertical() { _dongfeng->AnimateZuobanVertical(); }
    void AnimateYoubanHorizontal(bool open = true) { open ? _dongfeng->AnimateYoubanHorizontal(0, 1) : _dongfeng->AnimateYoubanHorizontal(1, 0); }
    void AnimateYoubanVertical() { _dongfeng->AnimateYoubanVertical(); }

public slots:
    //! Zoom to the extent of the data set in the scene
    void zoomToExtent();

signals:
    void pickedModuleChanged(const std::string& moduleName);

private:
    void StartTimer();
    void EndTimer(const QString& context = "time: ");
    void UpdateDepthRendering();

    DongfengVis* _dongfeng;
    vtkNew<vtkRenderer> _renderer;
    vtkNew<vtkLight> _light;
    vtkNew<vtkAxesActor> _axes;
    timerutil _tm;
    bool _isTiming;
    bool _isFullImported;
    double _highlightColor[3];
    double _modelColor[3];
    double _backgroundColor[3];
    bool _useDepthPeeling;
    int _maxPeels;
    double _occulusionRatio;
};

#endif // SCENEWIDGET_H
