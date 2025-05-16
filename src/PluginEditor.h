#pragma once

#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class ModernDial : public juce::Slider
{
public:
    ModernDial(const juce::String& labelText);
    void paint(juce::Graphics& g) override;

private:
    juce::String label;
};

class TimeButton : public juce::TextButton
{
public:
    TimeButton(const juce::String& buttonText);
    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    juce::Path powerSymbol;
};

class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor,
                                      public juce::Timer
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    AudioPluginAudioProcessor& processorRef;

    juce::Colour backgroundColour = juce::Colour(40, 40, 40);
    juce::Colour accentColour = juce::Colour(65, 172, 255);
    juce::Colour textColour = juce::Colour(229, 229, 229);

    TimeButton timeDiv1_1Button;
    TimeButton timeDiv1_2Button;
    TimeButton timeDiv1_4Button;
    TimeButton timeDiv1_8Button;
    TimeButton timeDiv1_16Button;
    TimeButton timeDiv1_32Button;
    TimeButton timeDiv1_64Button;
    ModernDial mixKnob;
    ModernDial pitchKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_1Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_2Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_4Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_8Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_16Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_32Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_64Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};