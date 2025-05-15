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
    // 画面描画
    void paint (juce::Graphics&) override;
    // レイアウト更新
    void resized() override;

private:
    // AudioProcessorへの参照（UIからパラメータやサンプルへアクセスするため）
    AudioPluginAudioProcessor& processorRef;

    // フィルターカットオフ用スライダー
    juce::Slider filterCutoffSlider;
    // フィルターレゾナンス用スライダー
    juce::Slider filterResonanceSlider;
    // スライダーとパラメータのバインド
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;

    // パッド数（UI用、サンプラーのボタン）
    static constexpr int numPads = 12;
    // パッドボタン配列
    juce::TextButton padButtons[numPads];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
