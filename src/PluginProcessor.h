#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

// 前方宣言
class AudioPluginAudioProcessorEditor;

class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // パラメーター取得用の関数
    float getGrainSize() const { return *grainSize; }
    float getGrainSpread() const { return *grainSpread; }
    float getGrainPitch() const { return *grainPitch; }
    float getReverbSize() const { return *reverbSize; }
    float getReverbDamping() const { return *reverbDamping; }
    float getReverbWidth() const { return *reverbWidth; }
    float getReverbMix() const { return *reverbMix; }

    // パラメーター管理用
    juce::AudioProcessorValueTreeState apvts;

private:
    // パラメーター
    juce::AudioParameterFloat* grainSize;     // グレインサイズ（10-500ms）
    juce::AudioParameterFloat* grainSpread;   // グレインの拡散（0-100%）
    juce::AudioParameterFloat* grainPitch;    // ピッチシフト（-12 to +12半音）
    juce::AudioParameterFloat* reverbSize;    // リバーブサイズ（0-1）
    juce::AudioParameterFloat* reverbDamping; // リバーブのダンピング（0-1）
    juce::AudioParameterFloat* reverbWidth;   // リバーブの幅（0-1）
    juce::AudioParameterFloat* reverbMix;     // リバーブのミックス（0-1）

    // オーディオ処理用のバッファ
    juce::AudioBuffer<float> grainBuffer;
    float lastSampleRate = 44100.0f;

    // グレイン処理用
    struct Grain {
        int startPosition;
        int length;
        float pitch;
        float pan;
        bool active;
    };
    std::vector<Grain> activeGrains;
    juce::Random random;

    // リバーブ
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;

    // パラメーターツリーの初期化
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // グレイン生成関数
    void triggerGrain();
    float getInterpolatedSample(const float* buffer, float position);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};