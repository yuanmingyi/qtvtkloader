#ifndef ANIMATIONSCENEOBSERVER_H
#define ANIMATIONSCENEOBSERVER_H

#include <vtkAnimationScene.h>
#include <vtkCommand.h>
#include <vtkRenderWindow.h>

class AnimationSceneObserver : public vtkCommand
{
public:
    static AnimationSceneObserver* New();

    void SetRenderMethod(std::function<void()> renderMethod) { _renderMethod = renderMethod; }
    virtual void Execute(vtkObject* caller, unsigned long event, void* calldata);

protected:
    AnimationSceneObserver();
    ~AnimationSceneObserver();

private:
    std::function<void()> _renderMethod;
};


#endif // ANIMATIONSCENEOBSERVER_H
