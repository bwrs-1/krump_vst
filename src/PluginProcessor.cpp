#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>

#ifndef JucePlugin_Name
#define JucePlugin_Name "Grain Reverb"
#endif

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "grainSize",     // パラメーターID
        "Grain Size",    // パラメーター名
        10.0f,          // 最小値（10ms）
        500.0f,         // 最大値（500ms）
        100.0f));       // デフォルト値

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "grainSpread",   // パラメーターID
        "Spread",        // パラメーター名
        0.0f,           // 最小値（0%）
        100.0f,         // 最大値（100%）
        50.0f));        // デフォルト値

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "grainPitch",    // パラメーターID
        "Pitch",         // パラメーター名
        -12.0f,         // 最小値（-12半音）
        12.0f,          // 最大値（+12半音）
        0.0f));         // デフォルト値

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "reverbSize",    // パラメーターID
        "Reverb Size",   // パラメーター名
        0.0f,           // 最小値
        1.0f,           // 最大値
        0.5f));         // デフォルト値

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "reverbDamping", // パラメーターID
        "Damping",       // パラメーター名
        0.0f,           // 最小値
        1.0f,           // 最大値
        0.5f));         // デフォルト値

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "reverbWidth",   // パラメーターID
        "Width",         // パラメーター名
        0.0f,           // 最小値
        1.0f,           // 最大値
        1.0f));         // デフォルト値

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "reverbMix",     // パラメーターID
        "Mix",           // パラメーター名
        0.0f,           // 最小値
        1.0f,           // 最大値
        0.3f));         // デフォルト値

    return layout;
}

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // パラメーターの取得
    grainSize = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("grainSize"));
    grainSpread = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("grainSpread"));
    grainPitch = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("grainPitch"));
    reverbSize = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("reverbSize"));
    reverbDamping = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("reverbDamping"));
    reverbWidth = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("reverbWidth"));
    reverbMix = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter("reverbMix"));
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    releaseResources();
}

void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    lastSampleRate = static_cast<float>(sampleRate);
    
    // グレインバッファの初期化
    grainBuffer.setSize(2, static_cast<int>(0.5f * sampleRate), false, true, true); // 最大500msのグレイン
    grainBuffer.clear();
    
    // グレイン処理の初期化
    activeGrains.clear();
    activeGrains.reserve(32); // 最大同時グレイン数を予約

    // リバーブの初期化
    reverbParams.roomSize = *reverbSize;
    reverbParams.damping = *reverbDamping;
    reverbParams.width = *reverbWidth;
    reverbParams.wetLevel = *reverbMix;
    reverbParams.dryLevel = 1.0f - *reverbMix;
    reverb.setParameters(reverbParams);
}

void AudioPluginAudioProcessor::releaseResources()
{
    grainBuffer.setSize(0, 0);
    activeGrains.clear();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    const float* inputL = buffer.getReadPointer(0);
    const float* inputR = buffer.getReadPointer(1);
    float* outputL = buffer.getWritePointer(0);
    float* outputR = buffer.getWritePointer(1);

    // バッファをクリア
    buffer.clear();

    // グレイン処理
    for (int i = 0; i < numSamples; ++i)
    {
        float outL = 0.0f;
        float outR = 0.0f;

        // アクティブなグレインの処理
        for (auto& grain : activeGrains)
        {
            if (grain.active)
            {
                float pos = grain.startPosition + (i * std::pow(2.0f, grain.pitch / 12.0f));
                if (pos < grain.startPosition + grain.length && pos >= 0)
                {
                    float sampleL = inputL[i];
                    float sampleR = inputR[i];
                    
                    // パンニング適用
                    outL += sampleL * (1.0f - grain.pan);
                    outR += sampleR * grain.pan;
                }
                else
                {
                    grain.active = false;
                }
            }
        }

        // 新しいグレインのトリガー
        if (random.nextFloat() < 0.1f) // グレインの密度調整
        {
            triggerGrain();
        }

        // 出力にミックス（クリッピング防止のためのゲイン調整）
        const float gain = 0.5f;
        outputL[i] = outL * gain;
        outputR[i] = outR * gain;
    }

    // リバーブパラメーターの更新
    reverbParams.roomSize = *reverbSize;
    reverbParams.damping = *reverbDamping;
    reverbParams.width = *reverbWidth;
    reverbParams.wetLevel = *reverbMix;
    reverbParams.dryLevel = 1.0f - *reverbMix;
    reverb.setParameters(reverbParams);

    // リバーブ処理
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

void AudioPluginAudioProcessor::triggerGrain()
{
    const float grainSizeMs = *grainSize;
    const float spreadAmount = *grainSpread / 100.0f;
    const float pitchShift = *grainPitch;

    Grain newGrain;
    newGrain.startPosition = 0;
    newGrain.length = static_cast<int>((grainSizeMs / 1000.0f) * lastSampleRate);
    newGrain.pitch = pitchShift + (random.nextFloat() * 2.0f - 1.0f) * spreadAmount;
    newGrain.pan = random.nextFloat();
    newGrain.active = true;

    // アクティブでないグレインを探して置き換え
    bool grainAdded = false;
    for (auto& grain : activeGrains)
    {
        if (!grain.active)
        {
            grain = newGrain;
            grainAdded = true;
            break;
        }
    }

    // アクティブでないグレインが見つからなければ新しく追加
    if (!grainAdded && activeGrains.size() < 32)
    {
        activeGrains.push_back(newGrain);
    }
}

float AudioPluginAudioProcessor::getInterpolatedSample(const float* buffer, float position)
{
    int pos1 = static_cast<int>(position);
    int pos2 = pos1 + 1;
    float frac = position - pos1;

    float sample1 = buffer[pos1];
    float sample2 = buffer[pos2];

    return sample1 + frac * (sample2 - sample1);
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
}

const juce::String AudioPluginAudioProcessor::getName() const
{
    return "Grain Loop";
}

bool AudioPluginAudioProcessor::acceptsMidi() const { return false; }
bool AudioPluginAudioProcessor::producesMidi() const { return false; }
bool AudioPluginAudioProcessor::isMidiEffect() const { return false; }
double AudioPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int AudioPluginAudioProcessor::getNumPrograms() { return 1; }
int AudioPluginAudioProcessor::getCurrentProgram() { return 0; }
void AudioPluginAudioProcessor::setCurrentProgram (int index) { juce::ignoreUnused(index); }
const juce::String AudioPluginAudioProcessor::getProgramName (int index) { juce::ignoreUnused(index); return {}; }
void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName) { juce::ignoreUnused(index, newName); }

void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}