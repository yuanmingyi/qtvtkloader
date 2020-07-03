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
    this->_staticActor = nullptr;
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

    std::set<vtkProp3D*> children, excluded;
    // load config
    if (configfile != nullptr)
    {
        LoadConfig(children, excluded, configfile);
    }

    // config root object
    _objRoot = vtkSmartPointer<vtkAssembly>::New();
    // update root object
    vtkNew<vtkAppendPolyData> appendData;
    bool hasOtherActors = false;
    for (auto it = _actors.begin(); it != _actors.end(); it++) {
        if (excluded.find(*it) == excluded.end() && children.find(*it) == children.end()) {
            // the actor is neither excluded nor a child
            auto mapper = (*it)->GetMapper();
            vtkPolyData* pd = dynamic_cast<vtkPolyData*>(mapper->GetInput());
            appendData->AddInputData(pd);
            hasOtherActors = true;
        }
    }
    if (hasOtherActors) {
        _staticActor = vtkSmartPointer<vtkActor>::New();
        vtkNew<vtkPolyDataMapper> otherMapper;
        otherMapper->SetInputConnection(appendData->GetOutputPort());
        otherMapper->ScalarVisibilityOff();
        _staticActor->SetMapper(otherMapper);
        _objRoot->AddPart(_staticActor);
        _combinedActors.push_back(_staticActor);
    }

    for (auto it = _assemblyMap.begin(); it != _assemblyMap.end(); it++) {
        if (children.find(it->second) == children.end()) {
            // the assembly is not a child
            _objRoot->AddPart(it->second);
        }
    }
    _assemblyMap["all"] = _objRoot;
}

vtkActor* ObjImporter::GetActor(const std::string& actorName)
{
    if (vtksys::SystemTools::StringStartsWith(actorName, "actor")) {
        size_t index = static_cast<size_t>(std::stoi(actorName.substr(5)));
        if (index < _actors.size()) {
            return _actors[index];
        }
    }
    return nullptr;
}

bool ObjImporter::LoadConfig(std::set<vtkProp3D*>& children, std::set<vtkProp3D*>& excluded, const char* configfile)
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
        if (tokens[0] == "exclude:") {
            for (size_t i = 1; i < tokens.size(); i++) {
                auto actor = GetActor(tokens[i]);
                if (actor) {
                    std::cout << "exclude " << tokens[i] << std::endl;
                    excluded.insert(actor);
                } else {
                    std::cerr << "actor not found: " << tokens[i] << std::endl;
                }
            }
        } else if (assembly == nullptr) {
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
                auto actor = GetActor(*it);
                if (actor) {
                    vtkPolyData* pd = dynamic_cast<vtkPolyData*>(actor->GetMapper()->GetInput());
                    appendData->AddInputData(pd);
                    children.insert(actor);
                    hasActorChildren = true;
                    std::cout << "\tadd " << *it << std::endl;
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
                mapper->ScalarVisibilityOff();
                actor->SetMapper(mapper);
                assembly->AddPart(actor);
                _combinedActors.push_back(actor);
            }
        }
    }

    ifs.close();
    return true;
}

void ObjImporter::ClearActors()
{
    for (auto it = _actors.begin(); it != _actors.end(); it++) {
        (*it)->UnRegister(nullptr);
    }
    _actors.clear();
    _combinedActors.clear();
    _staticActor = nullptr;
}
