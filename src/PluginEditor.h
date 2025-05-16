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

class XYPad : public juce::Component
{
public:
    XYPad();
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent&) override;
    void setPosition(float x, float y);
    
    std::function<void(float, float)> onPositionChanged;

private:
    void updatePosition(float x, float y);
    float currentX = 0.5f;
    float currentY = 0.5f;
    bool isDragging = false;
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

    // スタイリング用の色定義
    juce::Colour backgroundColour = juce::Colour(40, 40, 40);
    juce::Colour accentColour = juce::Colour(255, 154, 1);
    juce::Colour textColour = juce::Colour(200, 200, 200);

    // コントロール
    XYPad xyPad;
    TimeButton timeDiv1_2Button;
    TimeButton timeDiv1_4Button;
    TimeButton timeDiv1_8Button;
    TimeButton timeDiv1_16Button;
    ModernDial mixKnob;

    // パラメーターアタッチメント
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_2Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_4Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_8Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> timeDiv1_16Attachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};