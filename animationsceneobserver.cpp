#include "animationsceneobserver.h"

AnimationSceneObserver::AnimationSceneObserver()
{
    this->RenderWindow = nullptr;
}

AnimationSceneObserver::~AnimationSceneObserver()
{
    if (this->RenderWindow)
    {
        this->RenderWindow->UnRegister(this);
        this->RenderWindow = nullptr;
    }
}

AnimationSceneObserver* AnimationSceneObserver::New()
{
    return new AnimationSceneObserver;
}

void AnimationSceneObserver::SetRenderWindow(vtkRenderWindow* renWin)
{
    if (this->RenderWindow)
    {
        this->RenderWindow->UnRegister(this);
    }
    this->RenderWindow = renWin;
    this->RenderWindow->Register(this);
}

void AnimationSceneObserver::Execute(vtkObject* vtkNotUsed(caller), unsigned long event, void* vtkNotUsed(calldata))
{
    if (this->RenderWindow != nullptr)
    {
        switch (event)
        {
        case vtkCommand::AnimationCueTickEvent:
            this->RenderWindow->Render();
            break;
        }
    }
}
