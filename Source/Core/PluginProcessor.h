#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "../DSP/ReverbEffect.h"

class KrumpVSTAudioProcessor : public juce::AudioProcessor
{
public:
    KrumpVSTAudioProcessor();
    ~KrumpVSTAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Reverb parameters
    void setRoomSize(float value) { reverbEffect.setRoomSize(value); }
    void setDamping(float value) { reverbEffect.setDamping(value); }
    void setWetLevel(float value) { reverbEffect.setWetLevel(value); }
    void setDryLevel(float value) { reverbEffect.setDryLevel(value); }
    void setWidth(float value) { reverbEffect.setWidth(value); }
    void setFreezeMode(bool value) { reverbEffect.setFreezeMode(value); }

    // Parameter getters
    float getRoomSize() const { return reverbEffect.getParameter(0); }
    float getDamping() const { return reverbEffect.getParameter(1); }
    float getWetLevel() const { return reverbEffect.getParameter(2); }
    float getDryLevel() const { return reverbEffect.getParameter(3); }
    float getWidth() const { return reverbEffect.getParameter(4); }
    bool getFreezeMode() const { return reverbEffect.getParameter(5) > 0.5f; }

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // 波形バッファ参照用getter
    const float* getVisualizerBufferL() const { return visualizerBufferL; }
    const float* getVisualizerBufferR() const { return visualizerBufferR; }
    int getVisualizerBufferSize() const { return visualizerBufferSize; }

private:
    ReverbEffect reverbEffect;
    // 波形可視化用バッファ
    static constexpr int visualizerBufferSize = 1024;
    float visualizerBufferL[visualizerBufferSize] = {0};
    float visualizerBufferR[visualizerBufferSize] = {0};
    int visualizerWritePos = 0;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KrumpVSTAudioProcessor)
};