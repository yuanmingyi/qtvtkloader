#ifndef VTK_OBJ_READER_H
#define VTK_OBJ_READER_H

#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCallbackCommand.h>

class VtkObjLoader
{
public:
    typedef void (*RenderEventCallback)(vtkRenderer* renderer, void* statusData);

    VtkObjLoader();
    ~VtkObjLoader();

    vtkRenderer* GetRenderer() { return m_renderer; }
    RenderEventCallback GetCallback() { return m_callback; }
    void* GetCallbackStatusData() { return m_statusData; }

    void SetCallback(RenderEventCallback cb, void* statusData = nullptr);
    void RemoveObjects();
    void AddObjects(const std::string &filepath, int loadTimes = 1);

protected:
    vtkNew<vtkRenderer> m_renderer;
    RenderEventCallback m_callback;
    void* m_statusData;
};

#endif // VTK_OBJ_READER_H
