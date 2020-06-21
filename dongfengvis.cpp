#include <vtkMath.h>
#include <vtkTransform.h>
#include <vtkProp3DCollection.h>
#include <vtkActor.h>
#include <vtkTexture.h>
#include <vtkAnimationScene.h>
#include <vtksys/SystemTools.hxx>
#include "dongfenganimation.h"
#include "dongfengvis.h"

const std::string DongfengVis::None = "none";
const std::string DongfengVis::All = "all";
const std::string DongfengVis::Daofu = "daofu";
const std::string DongfengVis::Changbian = "changbian";
const std::string DongfengVis::Duanbian = "duanbian";
const std::string DongfengVis::Biantianxian = "biantianxian";
const std::string DongfengVis::Zuoban = "zuobian";
const std::string DongfengVis::Youban = "youban";
const std::string DongfengVis::Zuoban1 = "zuoban1";
const std::string DongfengVis::Youban1 = "youban1";
const std::string DongfengVis::Shengjianggan = "shengjianggan";
const std::string DongfengVis::Shengjianggan1 = "shengjianggan1";
const std::string DongfengVis::Shengjianggan2 = "shengjianggan2";
const std::string DongfengVis::Shengjianggan3 = "shengjianggan3";
const std::string DongfengVis::Shengjianggan4 = "shengjianggan4";

DongfengVis::HighlightArguments::HighlightArguments(const double* color, double opacity, double ambient, double diffuse, double specular, double specularPower)
{
    this->color[0] = color[0];
    this->color[1] = color[1];
    this->color[2] = color[2];
    this->opacity = opacity;
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->specularPower = specularPower;
}

DongfengVis::DongfengVis()
{
    _objImporter = new ObjImporter;
    _renderMethod = nullptr;
    _opacity = 1.0;
    _color[0] = 1.0;
    _color[1] = 1.0;
    _color[2] = 1.0;
}

DongfengVis::~DongfengVis()
{
    ClearTextures();
    delete _objImporter;
}

void DongfengVis::SetOpacity(double opacity)
{
    _opacity = opacity;
    auto actors = _objImporter->GetActors();
    for (auto it = actors.begin(); it != actors.end(); it++) {
        auto highlightMark = _highlightActorMap.find(*it);
        if (highlightMark != _highlightActorMap.end() && !highlightMark->second) {
            // actor is not highlight
            (*it)->GetProperty()->SetOpacity(_opacity);
        }
    }
}

void DongfengVis::SetColor(double r, double g, double b)
{
    _color[0] = r;
    _color[1] = g;
    _color[2] = b;
    auto actors = _objImporter->GetActors();
    for (auto it = actors.begin(); it != actors.end(); it++) {
        auto highlightMark = _highlightActorMap.find(*it);
        if (highlightMark != _highlightActorMap.end() && !highlightMark->second) {
            (*it)->GetProperty()->SetColor(r, g, b);
        }
    }
}

void DongfengVis::ImportObj(const std::string& filename, vtkRenderer* renderer, bool loadTexture)
{
    // remmove old actors
    auto configFile = vtksys::SystemTools::GetFilenamePath(filename) + "/" + vtksys::SystemTools::GetFilenameWithoutLastExtension(filename) + ".txt";
    std::cout << "config file path: " << configFile << std::endl;
    auto root = _objImporter->GetRootObject();
    if (root && renderer) {
        renderer->RemoveViewProp(root);
    }
    _objImporter->Import(filename.data(), configFile.data(), loadTexture);
    SaveActorProperties();
    root = _objImporter->GetRootObject();
    if (root && renderer) {
        renderer->AddViewProp(root);
    }
}

void DongfengVis::AnimateDaofu(double start, double end)
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderMethod(_renderMethod);
    animation->Add([this](double value) { this->RotateDaofu(std::move(value)); }, start, end, 0, 1);
    animation->Play();
    animation->Stop();
}

void DongfengVis::AnimateBiantianxian(double start, double end)
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderMethod(_renderMethod);
    animation->Add([this](double value) { this->RotateChangbian(std::move(value)); }, start, end);
    animation->Add([this](double value) { this->RotateDuanbian(std::move(value)); }, start, end);
    animation->Add([this](double value) { this->RotateBiantianxian(std::move(value)); }, start, end);
    animation->Play();
    animation->Stop();
}

void DongfengVis::AnimateZuobanHorizontal(double start, double end)
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderMethod(_renderMethod);
    animation->Add([this](double value) { this->RotateZuoban1(std::move(value)); }, start, end);
    animation->Play();
    animation->Stop();
}

void DongfengVis::AnimateZuobanVertical()
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderMethod(_renderMethod);
    animation->Add([this](double value) { this->RotateZuoban(std::move(value)); }, 0, -0.5, 0, 0.25);
    animation->Add([this](double value) { this->RotateZuoban(std::move(value)); }, -0.5, 0.5, 0.25, 0.75);
    animation->Add([this](double value) { this->RotateZuoban(std::move(value)); }, 0.5, 0, 0.75, 1);
    animation->Play();
    animation->Stop();
}

void DongfengVis::AnimateYoubanHorizontal(double start, double end)
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderMethod(_renderMethod);
    animation->Add([this](double value) { this->RotateYouban1(std::move(value)); }, start, end);
    animation->Play();
    animation->Stop();
}

void DongfengVis::AnimateYoubanVertical()
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderMethod(_renderMethod);
    animation->Add([this](double value) { this->RotateYouban(std::move(value)); }, 0, -0.5, 0, 0.25);
    animation->Add([this](double value) { this->RotateYouban(std::move(value)); }, -0.5, 0.5, 0.25, 0.75);
    animation->Add([this](double value) { this->RotateYouban(std::move(value)); }, 0.5, 0, 0.75, 1);
    animation->Play();
    animation->Stop();
}

void DongfengVis::AnimateShengjianggan(double start, double end)
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderMethod(_renderMethod);
    animation->Add([this](double value) { this->LiftShengjianggan(std::move(value)); }, start, end);
    animation->Add([this](double value) { this->LiftShengjianggan1(std::move(value)); }, start, end);
    animation->Add([this](double value) { this->LiftShengjianggan2(std::move(value)); }, start, end);
    animation->Add([this](double value) { this->LiftShengjianggan3(std::move(value)); }, start, end);
    animation->Add([this](double value) { this->LiftShengjianggan4(std::move(value)); }, start, end);
    animation->Play();
    animation->Stop();
}

void DongfengVis::AnimateHighlight(const std::string &moduleName, const HighlightArguments& args)
{
    vtkNew<DongfengAnimation> animation;
    Highlight(DongfengVis::None, args);
    if (_highlightFlags.find(moduleName) == _highlightFlags.end()) {
        // no module found
        return;
    }
    animation->SetRenderMethod(_renderMethod);
    animation->Add([this, moduleName, args](double value) {
        auto p = 5 * (value < 0.2 ? value : (value < 0.4 ? (0.4 - value) : (value < 0.6 ? (value - 0.4) : (value < 0.8 ? (0.8 - value) : (value - 0.8)))));
        HighlightArguments intArgs(args.color, args.opacity * p, args.ambient, args.diffuse, args.specular, args.specularPower);
        this->HighlightOn(moduleName, intArgs);
    }, 0, 1);
    animation->Play();
    animation->Stop();
}

void DongfengVis::Highlight(const std::string &moduleName, const HighlightArguments& args)
{
    auto assemblyMap = _objImporter->GetAssemblyMap();
    bool moduleExisted = false;
    for (auto it = assemblyMap.begin(); it != assemblyMap.end(); it++) {
        if (it->first != moduleName) {
            HighlightOff(it->first);
        } else {
            moduleExisted = true;
        }
    }
    if (moduleExisted) {
        HighlightOn(moduleName, args);
    }
}

void DongfengVis::HighlightOn(const std::string& moduleName, const HighlightArguments& args)
{
    auto module = _objImporter->GetAssemblyMap().at(moduleName);
    vtkNew<vtkPropCollection> collection;
    module->GetActors(collection);
    collection->InitTraversal();
    for (int i = 0; i < collection->GetNumberOfItems(); i++) {
        auto actor = vtkActor::SafeDownCast(collection->GetNextProp());
        actor->GetProperty()->SetOpacity(args.opacity);
        actor->GetProperty()->SetColor(args.color[0], args.color[1], args.color[2]);
        actor->GetProperty()->SetAmbient(args.ambient);
        actor->GetProperty()->SetDiffuse(args.diffuse);
        actor->GetProperty()->SetSpecular(args.specular);
        actor->GetProperty()->SetSpecularPower(args.specularPower);
        actor->SetTexture(nullptr);
        _highlightActorMap[actor] = true;
    }
    _highlightFlags[moduleName] = true;
}

void DongfengVis::HighlightOff(const std::string &moduleName)
{
    auto module = _objImporter->GetAssemblyMap().at(moduleName);
    vtkNew<vtkPropCollection> collection;
    module->GetActors(collection);
    collection->InitTraversal();
    for (int i = 0; i < collection->GetNumberOfItems(); i++) {
        auto actor = vtkActor::SafeDownCast(collection->GetNextProp());
        auto property = _properties.at(actor);
        auto texture = _textures.at(actor);
        actor->GetProperty()->DeepCopy(property);
        actor->GetProperty()->SetOpacity(_opacity);
        actor->GetProperty()->SetColor(_color);
        actor->SetTexture(texture);
        _highlightActorMap[actor] = false;
    }
    _highlightFlags[moduleName] = false;
}

void DongfengVis::RotateDaofu(double rate)
{
    auto daofu = _objImporter->GetAssemblyMap().at("daofu");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * 90;
    std::cout << "rotate daofu for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    double* origin = daofu->GetOrigin();
    tf->Translate(origin[0], origin[1], origin[2]);
    tf->RotateX(degree);
    tf->Translate(-origin[0], -origin[1], -origin[2]);
    daofu->SetUserTransform(tf);
}

void DongfengVis::RotateChangbian(double rate)
{
    auto changbian = _objImporter->GetAssemblyMap().at("changbian");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * 90;
    std::cout << "rotate changbian for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    double* origin = changbian->GetOrigin();
    tf->Translate(origin[0], origin[1], origin[2]);
    tf->RotateZ(degree);
    tf->Translate(-origin[0], -origin[1], -origin[2]);
    changbian->SetUserTransform(tf);
}

void DongfengVis::RotateDuanbian(double rate)
{
    auto duanbian = _objImporter->GetAssemblyMap().at("duanbian");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * -90;
    std::cout << "rotate duanbian for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    double* origin = duanbian->GetOrigin();
    tf->Translate(origin[0], origin[1], origin[2]);
    tf->RotateZ(degree);
    tf->Translate(-origin[0], -origin[1], -origin[2]);
    duanbian->SetUserTransform(tf);
}

void DongfengVis::RotateBiantianxian(double rate)
{
    auto biantianxian = _objImporter->GetAssemblyMap().at("biantianxian");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * -90;
    std::cout << "rotate biantianxian for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    double* origin = biantianxian->GetOrigin();
    tf->Translate(origin[0], origin[1], origin[2]);
    tf->RotateX(degree);
    tf->Translate(-origin[0], -origin[1], -origin[2]);
    biantianxian->SetUserTransform(tf);
}

void DongfengVis::RotateZuoban(double rate)
{
    auto zuoban = _objImporter->GetAssemblyMap().at("zuoban");
    auto degree = vtkMath::ClampValue(rate, -0.5, 0.5) * 90;
    std::cout << "rotate zuoban for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    double* origin = zuoban->GetOrigin();
    tf->Translate(origin[0], origin[1], origin[2]);
    tf->RotateX(degree);
    tf->Translate(-origin[0], -origin[1], -origin[2]);
    zuoban->SetUserTransform(tf);
}

void DongfengVis::RotateYouban(double rate)
{
    auto youban = _objImporter->GetAssemblyMap().at("youban");
    auto degree = vtkMath::ClampValue(rate, -0.5, 0.5) * 90;
    std::cout << "rotate youban for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    double* origin = youban->GetOrigin();
    tf->Translate(origin[0], origin[1], origin[2]);
    tf->RotateX(degree);
    tf->Translate(-origin[0], -origin[1], -origin[2]);
    youban->SetUserTransform(tf);
}

void DongfengVis::RotateZuoban1(double rate)
{
    auto zuoban1 = _objImporter->GetAssemblyMap().at("zuoban1");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * -90;
    std::cout << "rotate zuoban1 for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    double* origin = zuoban1->GetOrigin();
    tf->Translate(origin[0], origin[1], origin[2]);
    tf->RotateY(degree);
    tf->Translate(-origin[0], -origin[1], -origin[2]);
    zuoban1->SetUserTransform(tf);
}

void DongfengVis::RotateYouban1(double rate)
{
    auto youban1 = _objImporter->GetAssemblyMap().at("youban1");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * 90;
    std::cout << "rotate youban1 for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    double* origin = youban1->GetOrigin();
    tf->Translate(origin[0], origin[1], origin[2]);
    tf->RotateY(degree);
    tf->Translate(-origin[0], -origin[1], -origin[2]);
    youban1->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan(double rate)
{
    auto shengjianggan = _objImporter->GetAssemblyMap().at("shengjianggan");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 10;
    std::cout << "lift shengjianggan for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan1(double rate)
{
    auto shengjianggan1 = _objImporter->GetAssemblyMap().at("shengjianggan1");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 50;
    std::cout << "lift shengjianggan1 for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan1->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan2(double rate)
{
    auto shengjianggan2 = _objImporter->GetAssemblyMap().at("shengjianggan2");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 70;
    std::cout << "lift shengjianggan2 for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan2->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan3(double rate)
{
    auto shengjianggan3 = _objImporter->GetAssemblyMap().at("shengjianggan3");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 80;
    std::cout << "lift shengjianggan3 for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan3->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan4(double rate)
{
    auto shengjianggan4 = _objImporter->GetAssemblyMap().at("shengjianggan4");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 100;
    std::cout << "lift shengjianggan4 for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan4->SetUserTransform(tf);
}

void DongfengVis::SaveActorProperties()
{
    _properties.clear();
    ClearTextures();
    _moduleNames.clear();
    _highlightFlags.clear();
    _highlightActorMap.clear();
    _moduleNames.push_back(DongfengVis::None);
    auto actors = _objImporter->GetActors();
    for (auto it = actors.begin(); it != actors.end(); it++) {
        auto prop = vtkSmartPointer<vtkProperty>::New();
        prop->DeepCopy((*it)->GetProperty());
        _properties[*it] = prop;
        auto texture = (*it)->GetTexture();
        if (texture) {
            texture->Register(nullptr);
        }
        _textures[*it] = texture;
        _highlightActorMap[*it] = false;
        (*it)->GetProperty()->SetOpacity(_opacity);
        (*it)->GetProperty()->SetColor(_color);
    }
    auto modules = _objImporter->GetAssemblyMap();
    for (auto it = modules.begin(); it != modules.end(); it++) {
        _moduleNames.push_back(it->first);
        _highlightFlags[it->first] = false;
    }
}

void DongfengVis::ClearTextures()
{
    for (auto it = _textures.begin(); it != _textures.end(); it++) {
        auto texture = it->second;
        if (texture) {
            texture->UnRegister(nullptr);
        }
    }
    _textures.clear();
}
