#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include "vtk_obj_reader.h"
#include <QVTKOpenGLNativeWidget.h>

class SceneWidget : public QVTKOpenGLNativeWidget
{
    Q_OBJECT
public:
    explicit SceneWidget(QWidget *parent = nullptr);
    ~SceneWidget();

    void loadObj(const QString& fileName);

public slots:
    //! Zoom to the extent of the data set in the scene
    void zoomToExtent();

private:
    VtkObjLoader* m_loader;
};

#endif // SCENEWIDGET_H
