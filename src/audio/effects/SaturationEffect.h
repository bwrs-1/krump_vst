#pragma once

#include "Effect.h"
#include <juce_dsp/juce_dsp.h>

class SaturationEffect : public Effect
{
public:
    SaturationEffect();
    ~SaturationEffect() override = default;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    juce::StringArray getParameterNames() const override;
    juce::StringArray getParameterLabels() const override;
    juce::Array<float> getParameterRanges() const override;
    void setParameter(int parameterIndex, float value) override;
    float getParameter(int parameterIndex) const override;

    juce::String getName() const override { return "Saturation"; }
    juce::String getCategory() const override { return "Distortion"; }
    int getNumParameters() const override { return 2; }

    void saveToXml(juce::XmlElement& xml) const override;
    void loadFromXml(const juce::XmlElement& xml) override;

private:
    float drive = 0.5f;    // ドライブ量 (0-1)
    float wetDry = 0.5f;   // ウェット/ドライミックス (0-1)

    juce::dsp::Gain<float> inputGain;
    juce::dsp::WaveShaper<float> waveshaper;
    juce::dsp::Gain<float> outputGain;

    void updateParameters();
    static float saturationFunction(float x);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SaturationEffect)
}; 