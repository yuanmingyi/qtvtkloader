#ifndef ANIMATIONSCENEOBSERVER_H
#define ANIMATIONSCENEOBSERVER_H

#include <vtkAnimationScene.h>
#include <vtkCommand.h>
#include <vtkRenderWindow.h>

class AnimationSceneObserver : public vtkCommand
{
public:
    static AnimationSceneObserver* New();
    void SetRenderWindow(vtkRenderWindow* renWin);

    virtual void Execute(vtkObject* vtkNotUsed(caller), unsigned long event, void* vtkNotUsed(calldata));

protected:
    AnimationSceneObserver();
    ~AnimationSceneObserver();

    vtkRenderWindow* RenderWindow;
};


#endif // ANIMATIONSCENEOBSERVER_H
