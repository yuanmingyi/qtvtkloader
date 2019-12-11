/****************************************************************************
*   vtk_obj_importer.cxx
*
*   Created by:     Millan
*   Created on:     04/12/2019
*   Version:        1.0
*   Description:    
****************************************************************************/
#include "vtkNew.h"
#include <functional>
#include <vtkNamedColors.h>
#include <vtkOBJImporter.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTexture.h>
#include <vtkLight.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkOBJReader.h>
#include <vtkCallbackCommand.h>

#include "timerutil.h"
#include "vtk_obj_reader.h"


static void Callback(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    auto renderer = static_cast<vtkRenderer*>(caller);
    auto inst = static_cast<VtkObjLoader*>(clientData);
    if (inst && inst->GetCallback()) {
        inst->GetCallback()(renderer, inst->GetCallbackStatusData());
    }
}

static vtkSmartPointer<vtkActor> ReadObj(const std::string &filepath, double x, double y, double z)
{
    timerutil tm;

    vtkNew<vtkOBJReader> objReader;
    vtkNew<vtkPolyDataMapper> targetMapper;
    vtkNew<vtkActor> targetActor;
    vtkNew<vtkPolyData> obj;

    tm.start();

    objReader->SetFileName(filepath.c_str());
    objReader->Update();

    tm.end();
    /***************************************************************
    *   Add mappers, actors, renderer, and setup the scene
    ***************************************************************/
    std::cout << "\n**Load scene cost " << tm.msec() << "ms" << std::endl;

    targetMapper->SetInputData(objReader->GetOutput());
    targetActor->SetMapper(targetMapper);
    targetActor->SetPosition(x, y, z);
//    targetActor->GetProperty()->SetColor( 1, 1, 1 );
//    targetActor->GetProperty()->SetOpacity( 1.0 );
    targetActor->GetProperty()->SetPointSize(4);
//    targetActor->GetMapper()->ScalarVisibilityOff();
    return targetActor;
}

VtkObjLoader::VtkObjLoader()
{
    this->m_callback = nullptr;
    vtkNew<vtkCallbackCommand> callback;
    callback->SetCallback(::Callback);
    callback->SetClientData(this);
//    camera->SetViewUp(0,1,0);
//    camera->SetPosition(0,0,10);
//    camera->SetFocalPoint(0,0,0);
//    renderer->SetActiveCamera(camera);
    this->m_renderer->SetBackground(0.5, 0.5, 0.5);
    this->m_renderer->AddObserver(vtkCommand::EndEvent, callback);
}

VtkObjLoader::~VtkObjLoader()
{

}

void VtkObjLoader::SetCallback(VtkObjLoader::RenderEventCallback callback, void* statusData)
{
    this->m_callback = callback;
    this->m_statusData = statusData;
}

void VtkObjLoader::RemoveObjects()
{
    auto actorList = m_renderer->GetActors();
    actorList->InitTraversal();
    vtkActor *actor = actorList->GetNextActor();
    std::vector<vtkActor*> actors;
    while (actor != nullptr)
    {
        actors.push_back(actor);
        actor = actorList->GetNextActor();
    }
    for (size_t i = 0; i < actors.size(); i++)
    {
        m_renderer->RemoveActor(actors[i]);
    }
}

void VtkObjLoader::AddObjects(const std::string &filepath, int loadTimes)
{
    timerutil tm;
    tm.start();
    for (int i = 0; i < loadTimes; i++)
    {
        try {
            auto actor = ReadObj(filepath, i, i, i);
            this->m_renderer->AddActor(actor);
        } catch (std::exception& ex) {
            std::cerr << "Error: " << ex.what();
        }
    }
    tm.end();
    std::cout << "\n**Load all scenes cost " << tm.msec() << "ms" << std::endl;
}
