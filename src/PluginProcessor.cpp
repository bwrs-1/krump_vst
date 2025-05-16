#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_audio_plugin_client/juce_audio_plugin_client.h>

#ifndef JucePlugin_Name
#define JucePlugin_Name "HalfTime"
#endif

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // タイム分割の選択肢
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "timeDiv",      // パラメーターID
        "Time",         // パラメーター名
        juce::StringArray{"1/2", "1/4", "1/8", "1/16"},  // 選択肢
        0));           // デフォルト値（1/2）

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "mix",          // パラメーターID
        "Mix",          // パラメーター名
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f));      // デフォルト値

    return layout;
}

AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // パラメーターの取得と初期化
    timeDivParameter = apvts.getRawParameterValue("timeDiv");
    mixParameter = apvts.getRawParameterValue("mix");
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
    delayBufferLength = static_cast<int>(4.0 * sampleRate) + samplesPerBlock;
    delayBuffer.setSize(2, delayBufferLength, false, true, true);
    delayBuffer.clear();
    writePosition = 0;
}

void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    updateBufferSize(sampleRate, samplesPerBlock);
}

void AudioPluginAudioProcessor::releaseResources()
{
    clearBuffers();
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
    const juce::ScopedLock sl(processLock);
    juce::ScopedNoDenormals noDenormals;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    // バッファーの範囲チェックと更新
    if (delayBufferLength < numSamples * 4 || writePosition >= delayBufferLength)
    {
        updateBufferSize(lastSampleRate, numSamples);
    }

    // 入力をディレイバッファにコピー
    for (int channel = 0; channel < numChannels; ++channel)
    {
        const float* inputData = buffer.getReadPointer(channel);
        float* delayData = delayBuffer.getWritePointer(channel);
        
        for (int i = 0; i < numSamples; ++i)
        {
            const int pos = (writePosition + i) % delayBufferLength;
            delayData[pos] = inputData[i];
        }
    }

    // タイムストレッチのレート計算
    const float timeDiv = getTimeDivValue();
    float timeStretchRatio;
    switch (static_cast<int>(timeDiv))
    {
        case 0: timeStretchRatio = 2.0f;  break;
        case 1: timeStretchRatio = 4.0f;  break;
        case 2: timeStretchRatio = 8.0f;  break;
        case 3: timeStretchRatio = 16.0f; break;
        default: timeStretchRatio = 2.0f;
    }

    // ミックスの計算
    const float mixValue = juce::jlimit(0.0f, 100.0f, getMixValue());
    const float wetMix = mixValue * 0.01f;
    const float dryMix = 1.0f - wetMix;

    // 処理
    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        const float* delayData = delayBuffer.getReadPointer(channel);

        for (int i = 0; i < numSamples; ++i)
        {
            const float originalSample = channelData[i];
            
            // 読み取り位置の計算を改善
            float readPosition = writePosition + i - numSamples;
            readPosition /= timeStretchRatio;
            
            // 負の位置の処理を改善
            while (readPosition < 0)
                readPosition += delayBufferLength;

            // エイリアシングを防ぐための4点エルミート補間
            const int pos0 = static_cast<int>(readPosition - 1 + delayBufferLength) % delayBufferLength;
            const int pos1 = static_cast<int>(readPosition + delayBufferLength) % delayBufferLength;
            const int pos2 = static_cast<int>(readPosition + 1 + delayBufferLength) % delayBufferLength;
            const int pos3 = static_cast<int>(readPosition + 2 + delayBufferLength) % delayBufferLength;
            
            const float frac = readPosition - static_cast<float>(static_cast<int>(readPosition));
            const float frac2 = frac * frac;
            const float frac3 = frac2 * frac;

            // エルミート補間係数
            const float c0 = -0.5f * frac3 + 1.0f * frac2 - 0.5f * frac;
            const float c1 = 1.5f * frac3 - 2.5f * frac2 + 1.0f;
            const float c2 = -1.5f * frac3 + 2.0f * frac2 + 0.5f * frac;
            const float c3 = 0.5f * frac3 - 0.5f * frac2;

            // 補間された値を計算
            float stretchedSample = (delayData[pos0] * c0 +
                                   delayData[pos1] * c1 +
                                   delayData[pos2] * c2 +
                                   delayData[pos3] * c3);

            // ソフトクリッピングでノイズを抑制
            stretchedSample = std::tanh(stretchedSample);

            // ドライ/ウェットミックス
            channelData[i] = originalSample * dryMix + stretchedSample * wetMix;
        }
    }

    // 書き込み位置の更新
    writePosition = (writePosition + numSamples) % delayBufferLength;
}

float AudioPluginAudioProcessor::getInterpolatedSample(const float* buffer, float position, int bufferLength)
{
    int pos1 = static_cast<int>(position);
    int pos2 = (pos1 + 1) % bufferLength;
    float frac = position - pos1;

    pos1 = pos1 % bufferLength;
    if (pos1 < 0) pos1 += bufferLength;

    return linearInterpolate(buffer[pos1], buffer[pos2], frac);
}

float AudioPluginAudioProcessor::linearInterpolate(float a, float b, float t)
{
    return a + (b - a) * t;
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
    return timeDivParameter != nullptr ? timeDivParameter->load() : 0.0f;
}

float AudioPluginAudioProcessor::getMixValue() const noexcept
{
    return mixParameter != nullptr ? mixParameter->load() : 100.0f;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}