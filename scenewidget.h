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

    void ImportObj(const QString& fileName, bool loadTexture = true);
    const std::vector<std::string>& GetPickableItems() const{ return _dongfeng->GetModuleNames(); }

    bool IsImported() const { return _isFullImported; }

    void SetOpacity(double opacity);
    void PickModule(const std::string& moduleName);
    void SetLightIntensity(double intensity);
    void ShowAxes(bool show) { _axes->SetVisibility(show); }
    bool IsAxesShown() { return _axes->GetVisibility(); }

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

    DongfengVis* _dongfeng;
    vtkNew<vtkRenderer> _renderer;
    vtkNew<vtkLight> _light;
    vtkNew<vtkAxesActor> _axes;
    timerutil _tm;
    bool _isTiming;
    bool _isFullImported;
};

#endif // SCENEWIDGET_H
