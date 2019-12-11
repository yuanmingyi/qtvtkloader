#ifndef VTK_OBJ_READER_H
#define VTK_OBJ_READER_H

#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkCallbackCommand.h>

class VtkObjLoader
{
public:
    typedef void (*RenderEventCallback)(vtkRenderer* renderer);

    VtkObjLoader();
    ~VtkObjLoader();

    vtkRenderer* GetRenderer() { return m_renderer; }
    RenderEventCallback GetCallback() { return m_callback; }
    void SetCallback(RenderEventCallback cb);
    void RemoveObjects();
    void AddObjects(const std::string &filepath, int loadTimes = 1);

protected:
    vtkNew<vtkRenderer> m_renderer;
    RenderEventCallback m_callback;
};

#endif // VTK_OBJ_READER_H
