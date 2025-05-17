#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../Core/PluginProcessor.h"

// ダーク＋レッドのモダンUI LookAndFeel
class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CustomLookAndFeel();
    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height, float sliderPosProportional,
                         float rotaryStartAngle, float rotaryEndAngle, juce::Slider&) override;
    void drawLabel(juce::Graphics&, juce::Label&) override;
};

class KrumpVSTAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    KrumpVSTAudioProcessorEditor(KrumpVSTAudioProcessor&);
    ~KrumpVSTAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    KrumpVSTAudioProcessor& audioProcessor;
    juce::Font customFont;
    CustomLookAndFeel customLnf;
    // リバーブパラメータ用
    juce::Slider roomSizeSlider, dampingSlider, wetSlider, drySlider, widthSlider, freezeSlider;
    juce::Label roomSizeLabel, dampingLabel, wetLabel, dryLabel, widthLabel, freezeLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomSizeAttach, dampingAttach, wetAttach, dryAttach, widthAttach, freezeAttach;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KrumpVSTAudioProcessorEditor)
};