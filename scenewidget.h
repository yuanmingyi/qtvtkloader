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
    static void OnPickEvent(void* caller, unsigned long eventId, void* clientData, void* callData);

    explicit SceneWidget(QWidget *parent = nullptr);
    ~SceneWidget();

    void ImportObj(const QString& fileName);
    const std::vector<std::string>& GetPickableItems() const{ return _dongfeng->GetModuleNames(); }

    void PickModule(const std::string& moduleName);
    void SetLightIntensity(double intensity);
    void ShowAxes(bool show) { m_axes->SetVisibility(show); }
    bool IsAxesShown() { return m_axes->GetVisibility(); }

public slots:
    //! Zoom to the extent of the data set in the scene
    void zoomToExtent();

signals:
    void pickedModuleChanged(const std::string& moduleName);

private:
    DongfengVis* _dongfeng;
    vtkNew<DemoInteractorStyle> m_demoStyle;
    vtkNew<vtkRenderer> m_renderer;
    vtkNew<vtkLight> m_light;
    vtkNew<vtkAxesActor> m_axes;

    void UpdatePickedActor(int actorIndex);
};

#endif // SCENEWIDGET_H
