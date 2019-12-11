#include "vtk_obj_reader.h"
#include "timerutil.h"
#include <vtkRenderWindowInteractor.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkRenderer.h>

void cb(vtkRenderer* renderer, void* statusData)
{
    double ts = renderer->GetLastRenderTimeInSeconds();
    double fps = 1.0 / ts;
    std::cout << "render time: " << ts << "s fps: " << fps << std::endl;
}

int main(int argc, char** argv)
{
    if ( argc < 2 )
    {
        std::cout << "ERROR: Incorrect program usage." << std:: endl;
        std::cout << "Correct usage: " << argv[0] << " <obj_file_path> [load times]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string objPath = argv[1];
    int loadTimes = 1;
    if (argc > 2)
    {
        sscanf(argv[2], "%d", &loadTimes);
    }

    VtkObjLoader* loader = new VtkObjLoader;
    vtkNew<vtkRenderWindow> renWin;
    vtkNew<vtkRenderWindowInteractor> iren;

    loader->SetCallback(cb, nullptr);
    loader->AddObjects(objPath, loadTimes);

    timerutil tm;
    tm.start();
    renWin->SetSize(640, 480);
    renWin->AddRenderer(loader->GetRenderer());
    iren->SetRenderWindow(renWin);
    renWin->Render();
    tm.end();
    std::cout << "Render time: " << tm.msec() << "ms" << std::endl;

    iren->Start();

    delete loader;
    return EXIT_SUCCESS;
}
