#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/SP404LookAndFeel.h"

class DisplayLabel : public juce::Label
{
public:
    DisplayLabel(const juce::String& text = "");
    ~DisplayLabel() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setDisplayMode(bool isValueDisplay);
    void setValueText(const juce::String& value, const juce::String& suffix = "");

private:
    bool isValueMode = false;
    juce::String valueText;
    juce::String valueSuffix;
    SP404LookAndFeel labelLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DisplayLabel)
}; 