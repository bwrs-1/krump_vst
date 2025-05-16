#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/SP404LookAndFeel.h"

class EffectButton : public juce::TextButton
{
public:
    EffectButton(const juce::String& name = "");
    ~EffectButton() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setActive(bool shouldBeActive);
    bool isActive() const { return active; }

private:
    bool active = false;
    SP404LookAndFeel buttonLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectButton)
}; 