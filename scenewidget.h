#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QObject>
#include "timerutil.h"
#include "vtk_obj_reader.h"
#include "objimporter.h"
#include "demointeractorstyle.h"
#include <QVTKOpenGLNativeWidget.h>

class SceneWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    static void CompleteRender(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void OnPickEvent(void* caller, unsigned long eventId, void* clientData, void* callData);

    explicit SceneWidget(QWidget *parent = nullptr);
    ~SceneWidget();

    void LoadObj(const QString& fileName, int times = 1);
    void CompleteRender();

    void ImportObj(const QString& fileName);
    const std::vector<vtkActor*> GetActors() const{ return m_objImporter->GetActors(); }
    void PickActor(int index);
    void SetLightIntensity(double intensity);
    double* GetActorCenterBounds(int actorIndex, double* bounds = nullptr);
    void ShowAxes(bool show) { m_axes->SetVisibility(show); }
    bool IsAxesShown() { return m_axes->GetVisibility(); }

public slots:
    //! Zoom to the extent of the data set in the scene
    void zoomToExtent();

signals:
    void pickedActorChanged(int actorIndex);

private:
    ObjImporter *m_objImporter;
    vtkNew<DemoInteractorStyle> m_demoStyle;
    vtkNew<vtkRenderer> m_renderer;
    vtkNew<vtkLight> m_light;
    vtkNew<vtkAxesActor> m_axes;
    timerutil m_tm;
    bool m_startrender;

    void ClearProps();
    void RenderProps();
    void UpdatePickedActor(int actorIndex);
};

#endif // SCENEWIDGET_H
