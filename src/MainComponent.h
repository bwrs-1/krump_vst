#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

class MainComponent : public juce::Component
{
public:
    explicit MainComponent(AudioPluginAudioProcessor& p);
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    AudioPluginAudioProcessor& audioProcessor;

    // パッド用のボタン（2行4列）
    std::array<std::unique_ptr<juce::TextButton>, 8> pads;
    
    // コントロールノブ（3つ）とバリューノブ（1つ）
    std::array<std::unique_ptr<juce::Slider>, 3> controlKnobs;
    std::unique_ptr<juce::Slider> valueKnob;

    // LCD画面
    std::unique_ptr<juce::Label> lcdScreen;

    // レイアウトとデザインに必要な変数
    juce::Colour backgroundColour { juce::Colour(40, 40, 40) };
    juce::Colour padColour { juce::Colour(255, 159, 0) };
    juce::Colour textColour { juce::Colour(229, 229, 229) };

    // ユーティリティ関数
    void setupPads();
    void setupKnobs();
    void setupLCD();
    void updateLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
}; 