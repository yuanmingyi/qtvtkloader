#include "animationsceneobserver.h"

AnimationSceneObserver::AnimationSceneObserver()
{
    _renderMethod = nullptr;
    _ctx = "";
}

AnimationSceneObserver::~AnimationSceneObserver()
{
}

AnimationSceneObserver* AnimationSceneObserver::New()
{
    return new AnimationSceneObserver;
}

void AnimationSceneObserver::Execute(vtkObject* vtkNotUsed(caller), unsigned long event, void* vtkNotUsed(calldata))
{
    switch (event)
        {
        case vtkCommand::AnimationCueTickEvent:
            std::cout << "[AnimationCueTickEvent] render the window" << std::endl;
            if (_renderMethod) {
                _renderMethod(_ctx);
            }
            break;
        case vtkCommand::EndAnimationCueEvent:
            std::cout << "[EndAnimationCueEvent] render the window" << std::endl;
            if (_renderMethod) {
                _renderMethod(_ctx);
            }
            break;
        }
}
