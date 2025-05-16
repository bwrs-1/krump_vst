#pragma once

#include "PluginProcessor.h"

class ModernPadButton : public juce::Button
{
public:
    ModernPadButton() : juce::Button("Pad") {}

protected:
    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class ModernKnob : public juce::Slider
{
public:
    ModernKnob();
};

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    static constexpr int numPads = 16;
    static constexpr int numKnobs = 4;

    std::array<std::unique_ptr<ModernPadButton>, numPads> pads;
    std::array<std::unique_ptr<ModernKnob>, numKnobs> knobs;
    
    juce::Label displayLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};