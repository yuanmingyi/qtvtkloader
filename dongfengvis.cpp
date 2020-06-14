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

DongfengVis::DongfengVis(vtkRenderer* renderer)
{
    _objImporter = new ObjImporter;
    _renderer = renderer;
    _renderer->Register(nullptr);
}

DongfengVis::~DongfengVis()
{
    _renderer->UnRegister(nullptr);
    ClearTextures();
    delete _objImporter;
}

void DongfengVis::ImportObj(const std::string& filename)
{
    // remmove old actors
    ClearProps();
    auto configFile = vtksys::SystemTools::GetFilenamePath(filename) + "/" + vtksys::SystemTools::GetFilenameWithoutLastExtension(filename) + ".txt";
    std::cout << "config file path: " << configFile << std::endl;
    _objImporter->Import(filename.data(), configFile.data());
    SaveActorProperties();
    AddProps();
}

void DongfengVis::AnimateOpenDaofu()
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderWindow(_renderer->GetRenderWindow());
    animation->Add([this](double value) { this->RotateDaofu(std::move(value)); }, 0, 1);
    animation->Play();
    animation->Stop();
}

void DongfengVis::AnimateCloseDaofu()
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderWindow(_renderer->GetRenderWindow());
    animation->Add([this](double value) { this->RotateDaofu(std::move(value)); }, 1, 0);
    animation->Play();
    animation->Stop();
}

void DongfengVis::AnimateOpenBiantianxian()
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderWindow(_renderer->GetRenderWindow());
    animation->Add([this](double value) { this->RotateChangbian(std::move(value)); }, 0, 1);
    animation->Add([this](double value) { this->RotateDuanbian(std::move(value)); }, 0, 1);
    animation->Add([this](double value) { this->RotateBiantianxian(std::move(value)); }, 0, 1);
    animation->Play();
    animation->Stop();
}

void DongfengVis::AnimateCloseBiantianxian()
{
    vtkNew<DongfengAnimation> animation;
    animation->SetRenderWindow(_renderer->GetRenderWindow());
    animation->Add([this](double value) { this->RotateChangbian(std::move(value)); }, 1, 0);
    animation->Add([this](double value) { this->RotateDuanbian(std::move(value)); }, 1, 0);
    animation->Add([this](double value) { this->RotateBiantianxian(std::move(value)); }, 1, 0);
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
        actor->SetTexture(texture);
    }
    _highlightFlags[moduleName] = false;
}

void DongfengVis::RotateDaofu(double rate)
{
    auto daofu = _objImporter->GetAssemblyMap().at("daofu");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * 90;
    std::cout << "rotate daofu for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateX(degree);
    daofu->SetUserTransform(tf);
}

void DongfengVis::RotateChangbian(double rate)
{
    auto changbian = _objImporter->GetAssemblyMap().at("changbian");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * 90;
    std::cout << "rotate changbian for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateZ(degree);
    changbian->RotateZ(degree);
}

void DongfengVis::RotateDuanbian(double rate)
{
    auto duanbian = _objImporter->GetAssemblyMap().at("duanbian");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * -90;
    std::cout << "rotate duanbian for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateZ(degree);
    duanbian->SetUserTransform(tf);
}

void DongfengVis::RotateBiantianxian(double rate)
{
    auto biantianxian = _objImporter->GetAssemblyMap().at("biantianxian");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * -90;
    std::cout << "rotate biantianxian for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateX(degree);
    biantianxian->SetUserTransform(tf);
}

void DongfengVis::RotateZuoban(double rate)
{
    auto zuoban = _objImporter->GetAssemblyMap().at("zuoban");
    auto degree = vtkMath::ClampValue(rate, -0.5, 0.5) * 90;
    std::cout << "rotate zuoban for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateX(degree);
    zuoban->SetUserTransform(tf);
}

void DongfengVis::RotateYouban(double rate)
{
    auto youban = _objImporter->GetAssemblyMap().at("youban");
    auto degree = vtkMath::ClampValue(rate, -0.5, 0.5) * 90;
    std::cout << "rotate youban for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateX(degree);
    youban->SetUserTransform(tf);
}

void DongfengVis::RotateZuoban1(double rate)
{
    auto zuoban1 = _objImporter->GetAssemblyMap().at("zuoban1");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * -90;
    std::cout << "rotate zuoban1 for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateY(degree);
    zuoban1->SetUserTransform(tf);
}

void DongfengVis::RotateYouban1(double rate)
{
    auto youban1 = _objImporter->GetAssemblyMap().at("youban1");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * 90;
    std::cout << "rotate youban1 for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateY(degree);
    youban1->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan(double rate)
{
    auto shengjianggan = _objImporter->GetAssemblyMap().at("shengjianggan");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 40;
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
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 100;
    std::cout << "lift shengjianggan2 for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan2->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan3(double rate)
{
    auto shengjianggan3 = _objImporter->GetAssemblyMap().at("shengjianggan3");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 100;
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

void DongfengVis::ClearProps()
{
    auto rootObj = this->_objImporter->GetRootObject();
    this->_renderer->RemoveViewProp(rootObj);
}

void DongfengVis::AddProps()
{
    auto rootObj = _objImporter->GetRootObject();
    this->_renderer->AddViewProp(rootObj);
}

void DongfengVis::SaveActorProperties()
{
    _properties.clear();
    ClearTextures();
    _moduleNames.clear();
    _highlightFlags.clear();
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
