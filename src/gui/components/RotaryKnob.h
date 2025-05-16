#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/SP404LookAndFeel.h"

class RotaryKnob : public juce::Slider
{
public:
    RotaryKnob(const juce::String& labelText = "");
    ~RotaryKnob() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setLabelText(const juce::String& newText);
    void setValueSuffix(const juce::String& suffix);

private:
    juce::Label label;
    juce::String valueSuffix;
    SP404LookAndFeel knobLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnob)
}; 