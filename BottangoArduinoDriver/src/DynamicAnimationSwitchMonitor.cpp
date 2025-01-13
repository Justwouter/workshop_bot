#include "DynamicAnimationSwitchMonitor.h"
#include "../BottangoArduinoConfig.h"

#ifdef ENABLE_DYNAMIC_ANIMATION_SOURCE_SWITCH
namespace DynamicAnimationSwitch
{
    bool animationSwitchInitialized = false;

    bool shouldRunCommandStreams()
    {
        if (!animationSwitchInitialized)
        {
            pinMode(ANIMATION_STATE_SELECTION_PIN, INPUT);
            animationSwitchInitialized = true;
        }

#ifdef SELECT_EXPORTED_IS_HIGH
        return (digitalRead(ANIMATION_STATE_SELECTION_PIN));
#else
        return !(digitalRead(ANIMATION_STATE_SELECTION_PIN));
#endif
    }

} // namespace  Dynamic Animation Switch

#endif