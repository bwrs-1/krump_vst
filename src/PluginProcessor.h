#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h> // DSPエフェクト用

//==============================================================================
// メインのAudioProcessorクラス。音声処理やパラメータ管理を行う
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    AudioPluginAudioProcessor();  // コンストラクタ
    ~AudioPluginAudioProcessor() override; // デストラクタ

    //==============================================================================
    // 再生準備時に呼ばれる（バッファサイズやサンプルレートの初期化など）
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    // 再生停止時に呼ばれる（リソース解放など）
    void releaseResources() override;

    // 入出力チャンネルレイアウトがサポートされているか判定
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    using AudioProcessor::processBlock;

    // メインのオーディオ処理（ここでエフェクトやサンプル再生を行う）
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    // エディタ（UI）を生成
    juce::AudioProcessorEditor* createEditor() override;
    // エディタがあるかどうか
    bool hasEditor() const override;

    //==============================================================================
    // プラグイン情報
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    // プログラム（プリセット）管理
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    // プラグインの状態保存・復元
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // パッド数（サンプラー用）
    static const int numPads = 8;
    // 各パッドのサンプルバッファ
    std::array<juce::AudioSampleBuffer, numPads> padSamples;
    // サンプルがロード済みかどうか
    std::array<bool, numPads> padSampleLoaded { false };

    // パラメータレイアウト生成（AudioProcessorValueTreeState用）
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // パラメータ管理
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };

    // エフェクト
    juce::dsp::StateVariableTPTFilter<float> filter; // フィルター
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine { 44100 }; // ディレイ

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};
