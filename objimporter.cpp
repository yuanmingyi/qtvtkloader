#include "objimporter.h"
#include <vtkNew.h>
#include <vtkRenderer.h>
#include <vtkPolyData.h>
#include <vtkOBJImporter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkAssembly.h>
#include <vtkTexture.h>
#include <vtkRenderWindow.h>
#include <vtksys/SystemTools.hxx>
#include <set>
#include "stringutil.h"

ObjImporter::ObjImporter()
{
    this->_objRoot = nullptr;
    this->_importer = nullptr;
}

void ObjImporter::Import(const char* objfile, const char* configfile, bool loadTexture, const char* mtlfile, const char* texturedir)
{
    auto objdir = vtksys::SystemTools::GetFilenamePath(objfile);
    auto objname = vtksys::SystemTools::GetFilenameWithoutLastExtension(objfile);

    this->_importer = vtkSmartPointer<vtkOBJImporter>::New();
    this->_importer->SetFileName(objfile);

    if (loadTexture)
    {
        const char *mtlpath = (mtlfile == nullptr) ?
                    (objdir + "/" + objname + ".mtl").data() : mtlfile;
        const char *textpath = (texturedir == nullptr) ? objdir.data() : texturedir;
        this->_importer->SetFileNameMTL(mtlpath);
        this->_importer->SetTexturePath(textpath);
    }

    this->_importer->Update();

    auto actors = this->_importer->GetRenderer()->GetActors();
    actors->InitTraversal();
    int actorCount = actors->GetNumberOfItems();

    std::cout << "There are " << actorCount << " actors" << std::endl;

    this->_actors.clear();
    for (int i = 0; i < actorCount; ++i)
    {
        std::cout << this->_importer->GetOutputDescription(i) << std::endl;
        vtkActor* actor = actors->GetNextActor();

        actor->GetProperty()->SetAmbient(0.5);
        actor->GetProperty()->SetDiffuse(0.5);
        actor->GetProperty()->SetSpecular(0.5);

        this->_actors.push_back(actor);

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
        this->LoadConfig(children, configfile);
    }

    // config root object
    this->_objRoot = vtkSmartPointer<vtkAssembly>::New();
    // update root object
    for (auto it = this->_actors.begin(); it != this->_actors.end(); it++) {
        if (children.find(*it) == children.end()) {
            // the actor is not a child
            this->_objRoot->AddPart(*it);
        }
    }
    for (auto it = this->_assemblyMap.begin(); it != this->_assemblyMap.end(); it++) {
        if (children.find(it->second) == children.end()) {
            // the assembly is not a child
            this->_objRoot->AddPart(it->second);
        }
    }
}

bool ObjImporter::LoadConfig(std::set<vtkProp3D*>& children, const char* configfile)
{
    std::ifstream ifs(configfile);
    std::string line;
    std::string assemblyName;
    double originX, originY, originZ;
    std::vector<std::string> tokens;
    vtkSmartPointer<vtkAssembly> assembly = nullptr;

    this->_assemblyMap.clear();
    while (!ifs.eof())
    {
        std::getline(ifs, line);
        if (line.empty() || vtksys::SystemTools::StringStartsWith(line, "#")) {
            //  empty or comment line
            assembly = nullptr;
            continue;
        }
        vtksys::SystemTools::Split(line, tokens, ' ');
        if (assembly == nullptr) {
            assembly = vtkSmartPointer<vtkAssembly>::New();
            assemblyName = tokens[0];
            this->_assemblyMap[assemblyName] = assembly;
            vtksys::SystemTools::Split(tokens[1], tokens, ',');
            originX = std::stod(tokens[0]);
            originY = std::stod(tokens[1]);
            originZ = std::stod(tokens[2]);
            assembly->SetOrigin(originX, originY, originZ);
            std::cout << "Define assembly: " << assemblyName << " with origin: (" << originX << "," << originY << "," << originZ << ")" << std::endl;
        } else {
            // tokens are child actors/assemblies
            for (auto it = tokens.begin(); it != tokens.end(); it++) {
                // check actor
                if (vtksys::SystemTools::StringStartsWith(*it, "actor")) {
                    size_t index = static_cast<size_t>(std::stoi(it->substr(5)));
                    assembly->AddPart(this->_actors[index]);
                    children.insert(this->_actors[index]);
                    std::cout << "\tadd actor " << index << std::endl;
                } else {
                    auto childAssembly = this->_assemblyMap.find(*it);
                    if (childAssembly == this->_assemblyMap.end()) {
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
        }
    }

    return true;
}

