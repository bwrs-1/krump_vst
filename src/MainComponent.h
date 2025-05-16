#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>
#include "PluginProcessor.h"

// カスタムパッドボタンクラス
class SP404Pad : public juce::Button
{
public:
    SP404Pad(const juce::String& name) : juce::Button(name) 
    {
        setClickingTogglesState(true);
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
                    bool shouldDrawButtonAsDown) override;

private:
    juce::Path padShape;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SP404Pad)
};

// カスタムノブクラス
class SP404Knob : public juce::Slider
{
public:
    SP404Knob(const juce::String& labelText);
    void paint(juce::Graphics& g) override;

private:
    juce::String label;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SP404Knob)
};

// カスタムLCDディスプレイクラス
class SP404Display : public juce::Component
{
public:
    SP404Display();
    void paint(juce::Graphics& g) override;
    void setText(const juce::String& newText);

private:
    juce::String displayText;
    juce::Image displayBuffer;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SP404Display)
};

class MainComponent : public juce::Component,
                     public juce::Timer,
                     public juce::Button::Listener,
                     public juce::Slider::Listener
{
public:
    explicit MainComponent(AudioPluginAudioProcessor& p);
    ~MainComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    AudioPluginAudioProcessor& audioProcessor;

    // パッド用のボタン（2行4列）
    std::array<std::unique_ptr<SP404Pad>, 8> pads;
    
    // コントロールノブ（3つ）とバリューノブ（1つ）
    std::array<std::unique_ptr<SP404Knob>, 3> controlKnobs;
    std::unique_ptr<SP404Knob> valueKnob;

    // LCD画面
    std::unique_ptr<SP404Display> lcdScreen;

    // エフェクトボタン
    std::array<std::unique_ptr<juce::TextButton>, 4> effectButtons;

    // レイアウトとデザインに必要な変数
    juce::Colour backgroundColour { juce::Colour(40, 40, 40) };
    juce::Colour padColour { juce::Colour(255, 159, 0) };
    juce::Colour textColour { juce::Colour(229, 229, 229) };
    juce::Colour accentColour { juce::Colour(0, 172, 255) };

    // ユーティリティ関数
    void setupPads();
    void setupKnobs();
    void setupLCD();
    void setupEffectButtons();
    void updateLayout();
    void handlePadTrigger(int padIndex);
    void updateLCDDisplay();
    void applyTimeStretchEffect(float amount);

    // レスポンシブデザイン用の関数
    void calculateResponsiveLayout();
    float getScaleFactor() const;
    juce::Rectangle<int> getScaledBounds(const juce::Rectangle<int>& original) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
}; 