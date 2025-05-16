#pragma once

#include "PluginProcessor.h"

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        // ノブの回転角度を計算
        float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        
        // ノブの中心点を計算
        juce::Point<float> center(x + width * 0.5f, y + height * 0.5f);
        
        // ノブの背景を描画
        g.setColour(juce::Colours::darkgrey);
        g.fillEllipse(x + 2, y + 2, width - 4, height - 4);
        
        // ノブの外枠を描画
        g.setColour(juce::Colours::grey);
        g.drawEllipse(x + 2, y + 2, width - 4, height - 4, 2.0f);
        
        // ノブのポインターを描画
        juce::Path pointer;
        float pointerLength = width * 0.33f;
        float pointerThickness = 4.0f;
        
        pointer.addRectangle(-pointerThickness * 0.5f, -pointerLength, pointerThickness, pointerLength);
        
        g.setColour(juce::Colours::white);
        g.fillPath(pointer, juce::AffineTransform::rotation(angle).translated(center.x, center.y));
    }
};

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AudioPluginAudioProcessor& processorRef;
    CustomLookAndFeel customLookAndFeel;

    juce::Image backgroundImage;
    juce::Image knobImage;

    juce::Slider filterCutoffSlider;
    juce::Slider filterResonanceSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;

    static constexpr int numKnobs = 12;
    juce::Slider knobSliders[numKnobs];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};