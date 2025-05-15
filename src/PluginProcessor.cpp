#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// コンストラクタ：パラメータやエフェクトの初期化
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor (BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
#endif
    ),
    apvts(*this, nullptr, "PARAMETERS", {
        std::make_unique<juce::AudioParameterFloat>("filterCutoff", "Filter Cutoff", 20.0f, 20000.0f, 1000.0f),
        std::make_unique<juce::AudioParameterFloat>("filterResonance", "Filter Resonance", 0.1f, 2.0f, 0.7f)
    })
{
    // フィルター初期化
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    filter.setCutoffFrequency(1000.0f);
    filter.setResonance(0.7f);
}

// デストラクタ
AudioPluginAudioProcessor::~AudioPluginAudioProcessor() {}

//==============================================================================
// プラグイン情報
const juce::String AudioPluginAudioProcessor::getName() const { return JucePlugin_Name; }
bool AudioPluginAudioProcessor::acceptsMidi() const { return false; }
bool AudioPluginAudioProcessor::producesMidi() const { return false; }
bool AudioPluginAudioProcessor::isMidiEffect() const { return false; }
double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

//==============================================================================
// プログラム（プリセット）管理
int AudioPluginAudioProcessor::getNumPrograms() { return 1; }
int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }
void AudioPluginAudioProcessor::setCurrentProgram (int index) { juce::ignoreUnused(index); }
const juce::String AudioPluginAudioProcessor::getProgramName (int index) { juce::ignoreUnused(index); return {}; }
void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName) { juce::ignoreUnused(index, newName); }

//==============================================================================
// 再生準備時に呼ばれる（バッファサイズやサンプルレートの初期化など）
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    filter.prepare(spec);      // フィルター準備
    delayLine.prepare(spec);   // ディレイ準備
    delayLine.setDelay(500.0f); // ディレイタイム初期値
}

// 再生停止時に呼ばれる（リソース解放など）
void AudioPluginAudioProcessor::releaseResources() {}

// 入出力チャンネルレイアウトがサポートされているか判定
bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
#else
    // ステレオのみサポート
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif
    return true;
#endif
}

//==============================================================================
// メインのオーディオ処理（ここでエフェクトやサンプル再生を行う）
void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // パラメータ値を取得
    auto cutoff = apvts.getRawParameterValue("filterCutoff")->load();
    auto resonance = apvts.getRawParameterValue("filterResonance")->load();

    // フィルターにパラメータを反映
    filter.setCutoffFrequency(cutoff);
    filter.setResonance(resonance);

    // フィルター処理
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);

    // 必要に応じてディレイや他エフェクトもここで処理
}

//==============================================================================
// エディタ（UI）を生成
juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
}

// エディタがあるかどうか
bool AudioPluginAudioProcessor::hasEditor() const { return true; }

//==============================================================================
// プラグインの状態保存
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // パラメータの状態を保存
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

// プラグインの状態復元
void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // パラメータの状態を復元
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (apvts.state.getType()))
            apvts.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// パラメータレイアウト生成（AudioProcessorValueTreeState用）
juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // フィルターカットオフ
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterCutoff", "Filter Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f), 1000.0f));
    // フィルターレゾナンス
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterResonance", "Filter Resonance", juce::NormalisableRange<float>(0.1f, 2.0f), 0.7f));

    // 必要に応じて他のパラメータも追加

    return { params.begin(), params.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
