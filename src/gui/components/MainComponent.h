#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../PluginProcessor.h"
#include "../../presets/PresetManager.h"
#include "RotaryKnob.h"
#include "EffectButton.h"
#include "PresetButton.h"
#include "DisplayLabel.h"

class MainComponent : public juce::Component,
                     public juce::Button::Listener,
                     public juce::Slider::Listener
{
public:
    MainComponent(AudioPluginAudioProcessor& processor);
    ~MainComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    AudioPluginAudioProcessor& audioProcessor;
    std::unique_ptr<PresetManager> presetManager;

    // UI Components
    std::vector<std::unique_ptr<RotaryKnob>> controlKnobs;
    std::vector<std::unique_ptr<EffectButton>> effectButtons;
    std::vector<std::unique_ptr<PresetButton>> presetButtons;
    std::vector<std::unique_ptr<juce::TextButton>> pads;
    std::unique_ptr<DisplayLabel> display;

    // Layout helpers
    juce::Rectangle<int> getScaledBounds(float x, float y, float w, float h) const;
    void setupKnobs();
    void setupButtons();
    void setupPads();
    void updateDisplay();
    void updatePresetList();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
}; 