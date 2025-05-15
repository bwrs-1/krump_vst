// PluginEditor.h

#pragma once

#include "PluginProcessor.h"

//==============================================================================
// プラグインのUIクラス
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&); // コンストラクタ
    ~AudioPluginAudioProcessorEditor() override; // デストラクタ

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AudioPluginAudioProcessor& processorRef;

    // フィルターつまみ
    juce::Slider filterCutoffSlider;
    juce::Slider filterResonanceSlider;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;

    // ノブ表示用スライダー（旧パッド）
    static constexpr int numKnobs = 12;
    juce::Slider knobSliders[numKnobs]; // ← ここを追加

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
