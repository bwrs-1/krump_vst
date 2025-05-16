#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/SP404LookAndFeel.h"

class PresetButton : public juce::TextButton
{
public:
    PresetButton(const juce::String& name = "");
    ~PresetButton() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void setPresetNumber(int number);
    int getPresetNumber() const { return presetNumber; }

private:
    int presetNumber = 0;
    SP404LookAndFeel buttonLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetButton)
}; 