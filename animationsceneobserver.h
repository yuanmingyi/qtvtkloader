#ifndef ANIMATIONSCENEOBSERVER_H
#define ANIMATIONSCENEOBSERVER_H

#include <vtkAnimationScene.h>
#include <vtkCommand.h>
#include <vtkRenderWindow.h>
#include <functional>

class AnimationSceneObserver : public vtkCommand
{
public:
    static AnimationSceneObserver* New();

    void SetRenderMethod(std::function<void(const std::string&)> renderMethod, const std::string& ctx = "") { _renderMethod = renderMethod; _ctx = ctx; }
    virtual void Execute(vtkObject* caller, unsigned long event, void* calldata);

protected:
    AnimationSceneObserver();
    ~AnimationSceneObserver();

private:
    std::function<void(const std::string&)> _renderMethod;
    std::string _ctx;
};


#endif // ANIMATIONSCENEOBSERVER_H
