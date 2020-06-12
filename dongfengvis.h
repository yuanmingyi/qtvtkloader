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
    const std::string None = "none";
    const std::string All = "all";
    const std::string Daofu = "daofu";
    const std::string Changbian = "changbian";
    const std::string Duanbian = "duanbian";
    const std::string Biantianxian = "biantianxian";
    const std::string Zuoban = "zuobian";
    const std::string Youban = "youban";
    const std::string Zuoban1 = "zuoban1";
    const std::string Youban1 = "youban1";
    const std::string Shengjianggan = "shengjianggan";
    const std::string Shengjianggan1 = "shengjianggan1";
    const std::string Shengjianggan2 = "shengjianggan2";
    const std::string Shengjianggan3 = "shengjianggan3";
    const std::string Shengjianggan4 = "shengjianggan4";

    class HighlightArguments
    {
    public:
        double color[3];
        double opacity;
        double ambient;
        double diffuse;
        double specular;
        double specularPower;

        HighlightArguments(const double* color, double opacity = 1.0, double ambient = 0.5, double diffuse = 0.5, double specular = 0.5, double specularPower = 1);
    };

public:
    DongfengVis(vtkRenderer* renderer);
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

    void HighlightOn(const std::string& moduleName, const HighlightArguments& args);
    void HighlightOff(const std::string& moduleName);

    bool IsModuleHighlightOn(const std::string moduleName) { return _highlightFlags[moduleName]; }
    const std::vector<std::string>& GetModuleNames() const { return _moduleNames; }

private:
    vtkRenderer* _renderer;
    timerutil _tm;
    bool _startrender;

    ObjImporter *_objImporter;
    std::map<vtkActor*, vtkSmartPointer<vtkProperty>> _properties;
    std::map<vtkActor*, vtkSmartPointer<vtkTexture>> _textures;
    std::map<std::string, bool> _highlightFlags;
    std::vector<std::string> _moduleNames;

    vtkNew<CameraAnimationCue> _cameraCue;

    void UpdateActorProperties();
    void ClearProps();
    void RenderProps();
};

#endif // DONGFENGVIS_H
