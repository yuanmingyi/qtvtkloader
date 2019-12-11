#include "scenewidget.h"
#include "timerutil.h"
#include "vtk_obj_reader.h"
#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>

void cb(vtkRenderer* renderer, void* statusData)
{
    auto inst = static_cast<SceneWidget*>(statusData);
    inst->CompleteRender();
    double ts = renderer->GetLastRenderTimeInSeconds();
    double fps = 1.0 / ts;
    std::cout << "render time: " << ts << "s fps: " << fps << std::endl;
}

SceneWidget::SceneWidget(QWidget *parent)
  : QVTKOpenGLNativeWidget(parent)
{
    m_startrender = false;
    m_loader = new VtkObjLoader;
    m_loader->SetCallback(cb, this);
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    resize(800, 600);
    GetInteractor()->SetRenderWindow(window.Get());
    SetRenderWindow(window.Get());
    GetRenderWindow()->AddRenderer(m_loader->GetRenderer());
}

SceneWidget::~SceneWidget()
{
    delete m_loader;
}

void SceneWidget::loadObj(const QString& fileName, int times)
{
    m_loader->RemoveObjects();
    m_loader->AddObjects(fileName.toStdString(), times);
    m_startrender = true;
    m_tm.start();
    m_loader->GetRenderer()->ResetCamera();
    GetInteractor()->Render();
}

void SceneWidget::zoomToExtent()
{
    std::cout << "zoom to extent" << std::endl;
    m_loader->GetRenderer()->ResetCamera();
    GetInteractor()->Render();
}

void SceneWidget::CompleteRender()
{
    if (m_startrender) {
        m_startrender = false;
        m_tm.end();
        std::cout << "Render time: " << m_tm.msec() << "ms";
    }
}
