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

class XYPad : public juce::Component
{
public:
    XYPad();
    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;

    std::function<void(float, float)> onPositionChanged;

    void setPosition(float newX, float newY);
    float getXValue() const { return currentX; }
    float getYValue() const { return currentY; }

private:
    float currentX = 0.5f;
    float currentY = 0.5f;
    bool isDragging = false;

    void updatePosition(float x, float y);
};

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor,
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
    juce::Colour backgroundColour = juce::Colour(20, 22, 23);
    juce::Colour accentColour = juce::Colour(65, 172, 255);
    juce::Colour secondaryColour = juce::Colour(41, 43, 44);
    juce::Colour textColour = juce::Colour(229, 229, 229);

    // XYパッド
    XYPad xyPad;

    // リバーブコントロール
    ModernDial reverbSizeKnob;
    ModernDial reverbDampingKnob;
    ModernDial reverbWidthKnob;
    ModernDial reverbMixKnob;

    // パラメーター接続
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbSizeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbDampingAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbWidthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbMixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};