#include "dongfengvis.h"
#include <vtkMath.h>
#include <vtkTransform.h>

DongfengVis::DongfengVis()
{
    _objImporter = new ObjImporter();
}

DongfengVis::~DongfengVis()
{
    delete _objImporter;
}

void DongfengVis::ImportObj(const std::string& filename)
{
    // remmove old actors
    ClearProps();
    _tm.start();
    _objImporter->Import(filename.data());
    _tm.end();
    std::cout << "Import time: " << _tm.msec() << "ms" << std::endl;
    RenderProps();
}

void DongfengVis::ClearProps()
{
    auto actors = this->_objImporter->GetActors();
    for (auto it = actors.begin(); it != actors.end(); it++) {
        this->_renderer->RemoveViewProp(*it);
    }
//    auto rootObj = this->_objImporter->GetRootObject();
//    this->_renderer->RemoveViewProp(rootObj);
    _renderer->GetRenderWindow()->Render();
}

void DongfengVis::RenderProps()
{
    auto actors= this->_objImporter->GetActors();
    for (auto it = actors.begin(); it != actors.end(); it++) {
        this->_renderer->AddViewProp(*it);
    }
//    auto rootObj = _objImporter->GetRootObject();
//    this->_renderer->AddViewProp(rootObj);
    this->_renderer->ResetCamera();
    _tm.start();
    _startrender = true;
    _renderer->GetRenderWindow()->Render();
}

void DongfengVis::RotateDaofu(double rate)
{
    auto daofu = _assemblyMap.at("daofu");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * 90;
    std::cout << "rotate daofu for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateX(degree);
    daofu->SetUserTransform(tf);
}

void DongfengVis::RotateChangbian(double rate)
{
    auto changbian = _assemblyMap.at("changbian");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * 90;
    std::cout << "rotate changbian for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateZ(degree);
    changbian->RotateZ(degree);
}

void DongfengVis::RotateDuanbian(double rate)
{
    auto duanbian = _assemblyMap.at("duanbian");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * -90;
    std::cout << "rotate duanbian for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateZ(degree);
    duanbian->SetUserTransform(tf);
}

void DongfengVis::RotateBiantianxian(double rate)
{
    auto biantianxian = _assemblyMap.at("biantianxian");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * -90;
    std::cout << "rotate biantianxian for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateX(degree);
    biantianxian->SetUserTransform(tf);
}

void DongfengVis::RotateZuoban(double rate)
{
    auto zuoban = _assemblyMap.at("zuoban");
    auto degree = vtkMath::ClampValue(rate, -0.5, 0.5) * 90;
    std::cout << "rotate zuoban for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateX(degree);
    zuoban->SetUserTransform(tf);
}

void DongfengVis::RotateYouban(double rate)
{
    auto youban = _assemblyMap.at("youban");
    auto degree = vtkMath::ClampValue(rate, -0.5, 0.5) * 90;
    std::cout << "rotate youban for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateX(degree);
    youban->SetUserTransform(tf);
}

void DongfengVis::RotateZuoban1(double rate)
{
    auto zuoban1 = _assemblyMap.at("zuoban1");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * -90;
    std::cout << "rotate zuoban1 for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateY(degree);
    zuoban1->SetUserTransform(tf);
}

void DongfengVis::RotateYouban1(double rate)
{
    auto youban1 = _assemblyMap.at("youban1");
    auto degree = vtkMath::ClampValue(rate, 0.0, 1.0) * 90;
    std::cout << "rotate youban1 for " << degree << " degree" << std::endl;
    vtkNew<vtkTransform> tf;
    tf->RotateY(degree);
    youban1->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan(double rate)
{
    auto shengjianggan = _assemblyMap.at("shengjianggan");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 40;
    std::cout << "lift shengjianggan for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan1(double rate)
{
    auto shengjianggan1 = _assemblyMap.at("shengjianggan1");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 50;
    std::cout << "lift shengjianggan1 for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan1->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan2(double rate)
{
    auto shengjianggan2 = _assemblyMap.at("shengjianggan2");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 100;
    std::cout << "lift shengjianggan2 for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan2->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan3(double rate)
{
    auto shengjianggan3 = _assemblyMap.at("shengjianggan3");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 100;
    std::cout << "lift shengjianggan3 for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan3->SetUserTransform(tf);
}

void DongfengVis::LiftShengjianggan4(double rate)
{
    auto shengjianggan4 = _assemblyMap.at("shengjianggan4");
    auto distance = vtkMath::ClampValue(rate, 0.0, 1.0) * 100;
    std::cout << "lift shengjianggan4 for " << distance << std::endl;
    vtkNew<vtkTransform> tf;
    tf->Translate(0, distance, 0);
    shengjianggan4->SetUserTransform(tf);
}
