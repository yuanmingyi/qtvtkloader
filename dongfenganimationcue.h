#ifndef DONGFENGANIMATIONCUE_H
#define DONGFENGANIMATIONCUE_H

#include <vtkAnimationCue.h>
#include <vtkRenderWindow.h>
#include <functional>

class DongfengAnimationCue: public vtkAnimationCue
{
public:
    static DongfengAnimationCue* New()
    {
        return new DongfengAnimationCue();
    }

    void SetUpdateMethod(std::function<void(double)> updateStateFunc, double startValue, double endValue, double startTime, double endTime);

protected:
    DongfengAnimationCue();
    ~DongfengAnimationCue();

    virtual void StartCueInternal() override;
    virtual void TickInternal(double currentTime, double deltaTime, double clockTime) override;
    virtual void EndCueInternal() override;

private:
    std::function<void(double)> _updateStateFunc;
    double _startValue;
    double _endValue;
    double _startTime;
    double _endTime;
};

#endif // DONGFENGANIMATIONCUE_H
