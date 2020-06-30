#ifndef DONGFENGVIS_H
#define DONGFENGVIS_H

#include <functional>
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkProperty.h>
#include <vtkAxesActor.h>
#include <vtkRenderWindow.h>
#include "cameraanimationcue.h"
#include "objimporter.h"

class DongfengVis
{
public:
    static const std::string None;
    static const std::string All;
    static const std::string Daofu;
    static const std::string Changbian;
    static const std::string Duanbian;
    static const std::string Biantianxian;
    static const std::string Zuoban;
    static const std::string Youban;
    static const std::string Zuoban1;
    static const std::string Youban1;
    static const std::string Shengjianggan;
    static const std::string Shengjianggan1;
    static const std::string Shengjianggan2;
    static const std::string Shengjianggan3;
    static const std::string Shengjianggan4;

    class HighlightArguments
    {
    public:
        double color[3];
        double opacity;
        double ambient;
        double diffuse;
        double specular;
        double specularPower;

        HighlightArguments(const double* color, double opacity = 1, double ambient = 0.5, double diffuse = 0.5, double specular = 0.5, double specularPower = 1);
    };

public:
    DongfengVis();
    ~DongfengVis();

    // load obj data from file
    void ImportObj(const std::string& fileName, vtkRenderer* renderer, bool loadTexture = true, bool enableDepthSort = false);

    // predefine animations
    void AnimateDaofu(double start = 0, double end = 1);
    void AnimateBiantianxian(double start = 0, double end = 1);
    void AnimateShengjianggan(double start = 0, double end = 1);
    void AnimateZuobanHorizontal(double start = 0, double end = 1);
    void AnimateZuobanVertical();
    void AnimateYoubanHorizontal(double start = 0, double end = 1);
    void AnimateYoubanVertical();

    void AnimateHighlight(const std::string &moduleName, const HighlightArguments& args);

    // module movement, the arguments have a range of (0, 1) or (-0.5, 0.5)
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

    bool IsImported(const std::string& moduleName) { return _highlightFlags.find(moduleName) != _highlightFlags.end(); }

    // highlight the given module on and the other module off
    void Highlight(const std::string& moduleName, const HighlightArguments& args);

    // turn the highlight state of indicated module on with the given arguments
    void HighlightOn(const std::string& moduleName, const HighlightArguments& args);

    // turn the highlight state of indicated module off
    void HighlightOff(const std::string& moduleName);

    // return the highlight state associated with the module name
    bool IsModuleHighlightOn(const std::string moduleName) { return _highlightFlags.find(moduleName) == _highlightFlags.end() ? false : _highlightFlags.at(moduleName); }

    // return all the available modules' name
    const std::vector<std::string>& GetModuleNames() const { return _moduleNames; }

    // event handler for update the painting system
    void SetRenderMethod(std::function<void()> method) { _renderMethod = method; }

    double GetOpacity() const { return _opacity; }
    void SetOpacity(double opacity);
    void SetColor(double r, double g, double b);

    void EnableDepthSort(vtkRenderer* renderer);

private:
    ObjImporter *_objImporter;
    std::map<vtkActor*, vtkSmartPointer<vtkProperty>> _properties;
    std::map<vtkActor*, bool> _highlightActorMap;
    std::map<vtkActor*, vtkTexture*> _textures;
    std::map<std::string, bool> _highlightFlags;
    std::vector<std::string> _moduleNames;
    std::function<void()> _renderMethod;
    double _opacity;
    double _color[3];

    vtkNew<CameraAnimationCue> _cameraCue;

    void SaveActorProperties();
    void ClearTextures();
};

#endif // DONGFENGVIS_H
