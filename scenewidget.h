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

    void ImportObj(const QString& fileName);
    const std::vector<std::string>& GetPickableItems() const{ return _dongfeng->GetModuleNames(); }

    void PickModule(const std::string& moduleName);
    void SetLightIntensity(double intensity);
    void ShowAxes(bool show) { _axes->SetVisibility(show); }
    bool IsAxesShown() { return _axes->GetVisibility(); }

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
};

#endif // SCENEWIDGET_H
