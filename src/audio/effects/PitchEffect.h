#pragma once

#include "Effect.h"
#include <juce_dsp/juce_dsp.h>

class PitchEffect : public Effect
{
public:
    PitchEffect();
    ~PitchEffect() override = default;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    juce::StringArray getParameterNames() const override;
    juce::StringArray getParameterLabels() const override;
    juce::Array<float> getParameterRanges() const override;
    void setParameter(int parameterIndex, float value) override;
    float getParameter(int parameterIndex) const override;

    juce::String getName() const override { return "Pitch"; }
    juce::String getCategory() const override { return "Pitch"; }
    int getNumParameters() const override { return 2; }

    void saveToXml(juce::XmlElement& xml) const override;
    void loadFromXml(const juce::XmlElement& xml) override;

private:
    float pitch = 0.0f;     // ピッチシフト量（半音単位: -12 to +12）
    float fineTune = 0.0f;  // 微調整（セント単位: -50 to +50）

    juce::AudioBuffer<float> pitchBuffer;
    int bufferLength = 0;
    int writePosition = 0;
    float lastSampleRate = 44100.0f;

    void updatePitchShift();
    float getPitchRatio() const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PitchEffect)
}; 