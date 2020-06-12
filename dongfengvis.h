#ifndef DONGFENGVIS_H
#define DONGFENGVIS_H

#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkProperty.h>
#include <vtkAxesActor.h>
#include "cameraanimationcue.h"
#include "objimporter.h"
#include "timerutil.h"

class DongfengVis
{
public:
    DongfengVis();
    ~DongfengVis();

    void ImportObj(const std::string& fileName);
    void RotateDaofu(double);
    void RotateChangbian(double);
    void RotateDuanbian(double);
    void RotateBiantianxian(double);
    void RotateZuoban(double);
    void RotateYouban(double);
    void RotateZuoban1(double);
    void RotateYouban1(double);
    void LiftShengjianggan(double);
    void LiftShengjianggan1(double);
    void LiftShengjianggan2(double);
    void LiftShengjianggan3(double);
    void LiftShengjianggan4(double);

private:
    vtkNew<vtkRenderer> _renderer;
    timerutil _tm;
    bool _startrender;

    ObjImporter *_objImporter;
    std::vector<vtkActor*> _actors;
    vtkActor* _selectedActor;
    vtkNew<vtkProperty> _selectedActorProperty;
    vtkNew<vtkAxesActor> _axes;
    vtkSmartPointer<vtkTexture> _selectedActorTexture;
    vtkNew<CameraAnimationCue> _cameraCue;
    vtkSmartPointer<vtkAssembly> _objRoot;
    std::map<std::string, vtkSmartPointer<vtkAssembly>> _assemblyMap;

    void ClearProps();
    void RenderProps();
};

#endif // DONGFENGVIS_H
