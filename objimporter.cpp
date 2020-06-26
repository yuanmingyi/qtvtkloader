#include "objimporter.h"
#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkOBJImporter.h>
#include <vtkPolyDataMapper.h>
#include <vtkAppendPolyData.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkTexture.h>
#include <vtkRenderWindow.h>
#include <vtkDepthSortPolyData.h>
#include <vtksys/SystemTools.hxx>
#include <set>
#include "stringutil.h"

ObjImporter::ObjImporter()
{
    this->_objRoot = nullptr;
}

void ObjImporter::Import(const char* objfile, const char* configfile, bool loadTexture, const char* mtlfile, const char* texturedir)
{
    auto objdir = vtksys::SystemTools::GetFilenamePath(objfile);
    auto objname = vtksys::SystemTools::GetFilenameWithoutLastExtension(objfile);

    vtkNew<vtkOBJImporter> importer;
    importer->SetFileName(objfile);

    const char *mtlpath = (mtlfile == nullptr) ? (objdir + "/" + objname + ".mtl").data() : mtlfile;
    importer->SetFileNameMTL(mtlpath);
    if (loadTexture) {
        const char *textpath = (texturedir == nullptr) ? objdir.data() : texturedir;
        importer->SetTexturePath(textpath);
    }

    importer->Update();

    auto actors = importer->GetRenderer()->GetActors();
    actors->InitTraversal();
    int actorCount = actors->GetNumberOfItems();

    std::cout << "There are " << actorCount << " actors" << std::endl;

    ClearActors();
    for (int i = 0; i < actorCount; ++i)
    {
        std::cout << importer->GetOutputDescription(i) << std::endl;
        vtkActor* actor = actors->GetNextActor();

        actor->Register(nullptr);
        _actors.push_back(actor);

        // OBJImporter turns texture interpolation off
        auto texture = actor->GetTexture();
        if (texture)
        {
            std::cout << "Has texture\n";
            texture->InterpolateOn();
        }

        vtkPolyData* pd = dynamic_cast<vtkPolyData*>(actor->GetMapper()->GetInput());
        vtkPolyDataMapper* mapper = dynamic_cast<vtkPolyDataMapper*>(actor->GetMapper());
        mapper->SetInputData(pd);
    }

    std::set<vtkProp3D*> children;
    // load config
    if (configfile != nullptr)
    {
        LoadConfig(children, configfile);
    }

    // config root object
    _objRoot = vtkSmartPointer<vtkAssembly>::New();
    // update root object
    vtkNew<vtkAppendPolyData> appendData;
    bool hasOtherActors = false;
    for (auto it = _actors.begin(); it != _actors.end(); it++) {
        if (children.find(*it) == children.end()) {
            // the actor is not a child
            auto mapper = (*it)->GetMapper();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(mapper->GetInput());
            appendData->AddInputData(pd);
            hasOtherActors = true;
            //_objRoot->AddPart(*it);
        }
    }
    if (hasOtherActors) {
        vtkNew<vtkActor> otherActor;
        vtkNew<vtkPolyDataMapper> otherMapper;
        otherMapper->SetInputConnection(appendData->GetOutputPort());
        otherActor->SetMapper(otherMapper);
        otherActor->GetProperty()->SetOpacity(0.995);
        _objRoot->AddPart(otherActor);
        _combinedActors.push_back(otherActor);
    }

    for (auto it = _assemblyMap.begin(); it != _assemblyMap.end(); it++) {
        if (children.find(it->second) == children.end()) {
            // the assembly is not a child
            _objRoot->AddPart(it->second);
        }
    }
    _assemblyMap["all"] = _objRoot;
}

bool ObjImporter::LoadConfig(std::set<vtkProp3D*>& children, const char* configfile)
{
    std::ifstream ifs(configfile);
    std::string line;
    std::string assemblyName;
    double originX, originY, originZ;
    vtkSmartPointer<vtkAssembly> assembly = nullptr;

    _assemblyMap.clear();
    while (!ifs.eof())
    {
        std::getline(ifs, line);
        if (line.empty() || vtksys::SystemTools::StringStartsWith(line, "#")) {
            //  empty or comment line
            assembly = nullptr;
            continue;
        }
        std::cout << "read line: " << line << std::endl;
        std::vector<std::string> tokens;
        vtksys::SystemTools::Split(line, tokens, ' ');
        if (assembly == nullptr) {
            assembly = vtkSmartPointer<vtkAssembly>::New();
            assemblyName = tokens[0];
            _assemblyMap[assemblyName] = assembly;
            std::vector<std::string> numbers;
            vtksys::SystemTools::Split(tokens[1], numbers, ',');
            originX = std::stod(numbers[0]);
            originY = std::stod(numbers[1]);
            originZ = std::stod(numbers[2]);
            assembly->SetOrigin(originX, originY, originZ);
            std::cout << "Define assembly: " << assemblyName << " with origin: (" << originX << "," << originY << "," << originZ << ")" << std::endl;
        } else {
            // tokens are child actors/assemblies
            vtkNew<vtkAppendPolyData> appendData;
            bool hasActorChildren = false;
            for (auto it = tokens.begin(); it != tokens.end(); it++) {
                // check actor
                if (vtksys::SystemTools::StringStartsWith(*it, "actor")) {
                    size_t index = static_cast<size_t>(std::stoi(it->substr(5)));
                    auto actor = _actors[index];
                    vtkPolyData* pd = dynamic_cast<vtkPolyData*>(actor->GetMapper()->GetInput());
                    appendData->AddInputData(pd);
                    children.insert(actor);
                    hasActorChildren = true;
                    std::cout << "\tadd actor " << index << std::endl;
                } else {
                    auto childAssembly = _assemblyMap.find(*it);
                    if (childAssembly == _assemblyMap.end()) {
                        // undefined assembly
                        std::cerr << "assembly not defined: " << *it << std::endl;
                        return false;
                    } else if (childAssembly->second == assembly) {
                        // self contain
                        std::cerr << "assembly cannot add itself: " << assemblyName << std::endl;
                        return false;
                    } else {
                        assembly->AddPart(childAssembly->second);
                        children.insert(childAssembly->second);
                    }
                }
            }
            if (hasActorChildren) {
                vtkNew<vtkActor> actor;
                vtkNew<vtkPolyDataMapper> mapper;
                mapper->SetInputConnection(appendData->GetOutputPort());
                actor->SetMapper(mapper);
                actor->GetProperty()->SetOpacity(0.995);
                assembly->AddPart(actor);
                _combinedActors.push_back(actor);
            }
        }
    }

    return true;
}

void ObjImporter::ClearActors()
{
    for (auto it = _actors.begin(); it != _actors.end(); it++) {
        (*it)->UnRegister(nullptr);
    }
    _actors.clear();
    _combinedActors.clear();
}
