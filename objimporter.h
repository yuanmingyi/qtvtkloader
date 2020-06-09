#ifndef OBJIMPORTER_H
#define OBJIMPORTER_H

#include <vtkSmartPointer.h>
#include <vtkOBJImporter.h>
#include <vtkAssembly.h>
#include <vtkRenderer.h>
#include <map>
#include <set>

class ObjImporter
{
public:
    const std::vector<vtkActor*> GetActors() const { return this->_actors; }
    const std::map<std::string, vtkSmartPointer<vtkAssembly>> GetAssemblyMap() const { return this->_assemblyMap; }
    vtkAssembly* GetRootObject() const { return this->_objRoot; }

    void Import(const char* objfile, const char* configfile = nullptr, bool loadTexture = true, const char* mtlfile = nullptr, const char* texturedir = nullptr);

public:
    ObjImporter();

private:
    vtkSmartPointer<vtkOBJImporter> _importer;
    vtkSmartPointer<vtkAssembly> _objRoot;
    std::map<std::string, vtkSmartPointer<vtkAssembly>> _assemblyMap;
    std::vector<vtkActor*> _actors;

    bool LoadConfig(std::set<vtkProp3D*>& children, const char* configfile);
};

#endif // OBJIMPORTER_H
