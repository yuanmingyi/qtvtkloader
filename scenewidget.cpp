#include "scenewidget.h"
#include "vtk_obj_reader.h"
#include <vtkSmartPointer.h>
#include <vtkGenericOpenGLRenderWindow.h>

void cb(vtkRenderer* renderer)
{
    double ts = renderer->GetLastRenderTimeInSeconds();
    double fps = 1.0 / ts;
    std::cout << "render time: " << ts << "s fps: " << fps << std::endl;
}

SceneWidget::SceneWidget(QWidget *parent)
  : QVTKOpenGLNativeWidget(parent)
{
    m_loader = new VtkObjLoader;
    m_loader->SetCallback(cb);
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

void SceneWidget::loadObj(const QString& fileName)
{
    m_loader->RemoveObjects();
    m_loader->AddObjects(fileName.toStdString(), 1);
    m_loader->GetRenderer()->ResetCamera();
    update();
}

void SceneWidget::zoomToExtent()
{
    m_loader->GetRenderer()->ResetCamera();
    update();
}
