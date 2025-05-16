#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "audio/effects/ReverbEffect.h"
#include "audio/effects/DelayEffect.h"
#include "audio/effects/FilterEffect.h"
#include "audio/effects/PitchEffect.h"
#include "audio/effects/SaturationEffect.h"

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "timeDiv",
        "Time",
        juce::StringArray{"1/1", "1/2", "1/4", "1/8", "1/16", "1/32", "1/64"},
        1));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "mix",
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "pitch",
        "Pitch",
        juce::NormalisableRange<float>(-12.0f, 12.0f, 0.1f),
        0.0f));

    return layout;
}

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // エフェクトチェーンの初期化
    effectChain.addEffect(std::make_unique<ReverbEffect>());
    effectChain.addEffect(std::make_unique<DelayEffect>());
    effectChain.addEffect(std::make_unique<FilterEffect>());
    effectChain.addEffect(std::make_unique<PitchEffect>());
    effectChain.addEffect(std::make_unique<SaturationEffect>());

    timeDivParameter = apvts.getRawParameterValue("timeDiv");
    mixParameter = apvts.getRawParameterValue("mix");
    pitchParameter = apvts.getRawParameterValue("pitch");
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
    clearBuffers();
}

void AudioPluginAudioProcessor::clearBuffers()
{
    const juce::ScopedLock sl(processLock);
    delayBuffer.clear();
    delayBufferLength = 0;
    writePosition = 0;
}

void AudioPluginAudioProcessor::updateBufferSize(double sampleRate, int samplesPerBlock)
{
    const juce::ScopedLock sl(processLock);
    lastSampleRate = static_cast<float>(sampleRate);
    delayBufferLength = static_cast<int>(8.0 * sampleRate) + samplesPerBlock;
    delayBuffer.setSize(2, delayBufferLength, false, true, true);
    delayBuffer.clear();
    writePosition = 0;

    // アンチエイリアスフィルターの設定
    *antiAliasFilter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, sampleRate * 0.45f);
}

void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    effectChain.prepare(spec);
    antiAliasFilter.prepare(spec);
    updateBufferSize(sampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // エフェクトチェーンの処理
    effectChain.process(buffer);

    // アンチエイリアスフィルターの処理
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    antiAliasFilter.process(context);
}

void AudioPluginAudioProcessor::releaseResources()
{
    effectChain.reset();
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
    return true;
}

float AudioPluginAudioProcessor::hermiteInterpolate(float y0, float y1, float y2, float y3, float t)
{
    float c0 = y1;
    float c1 = 0.5f * (y2 - y0);
    float c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
    float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
    return ((c3 * t + c2) * t + c1) * t + c0;
}

float AudioPluginAudioProcessor::getInterpolatedSample(const float* buffer, float position, int bufferLength)
{
    int pos1 = static_cast<int>(position);
    int pos2 = (pos1 + 1) % bufferLength;
    float frac = position - pos1;

    pos1 = pos1 % bufferLength;
    if (pos1 < 0) pos1 += bufferLength;

    return buffer[pos1] + frac * (buffer[pos2] - buffer[pos1]);
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
}

const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
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

float AudioPluginAudioProcessor::getTimeDivValue() const noexcept
{
    return timeDivParameter != nullptr ? timeDivParameter->load() : 1.0f;
}

float AudioPluginAudioProcessor::getMixValue() const noexcept
{
    return mixParameter != nullptr ? mixParameter->load() : 100.0f;
}

float AudioPluginAudioProcessor::getPitchValue() const noexcept
{
    return pitchParameter != nullptr ? pitchParameter->load() : 0.0f;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}

// エフェクト管理関数の実装
void AudioPluginAudioProcessor::addEffect(std::unique_ptr<Effect> effect)
{
    effectChain.addEffect(std::move(effect));
}

void AudioPluginAudioProcessor::removeEffect(int index)
{
    effectChain.removeEffect(index);
}

void AudioPluginAudioProcessor::moveEffect(int fromIndex, int toIndex)
{
    effectChain.moveEffect(fromIndex, toIndex);
}

Effect* AudioPluginAudioProcessor::getEffect(int index)
{
    return effectChain.getEffect(index);
}

int AudioPluginAudioProcessor::getNumEffects() const
{
    return effectChain.getNumEffects();
}