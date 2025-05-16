#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

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

    juce::AudioProcessorValueTreeState apvts;

private:
    std::atomic<float>* timeDivParameter = nullptr;
    std::atomic<float>* mixParameter = nullptr;
    std::atomic<float>* pitchParameter = nullptr;

    juce::AudioBuffer<float> delayBuffer;
    int delayBufferLength = 0;
    int writePosition = 0;
    float lastSampleRate = 44100.0f;

    juce::CriticalSection processLock;
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                  juce::dsp::IIR::Coefficients<float>> antiAliasFilter;

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    float getTimeDivValue() const noexcept;
    float getMixValue() const noexcept;
    float getPitchValue() const noexcept;

    void updateBufferSize(double sampleRate, int samplesPerBlock);
    void clearBuffers();
    float getInterpolatedSample(const float* buffer, float position, int bufferLength);
    float hermiteInterpolate(float y0, float y1, float y2, float y3, float t);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)
};