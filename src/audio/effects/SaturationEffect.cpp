#include "SaturationEffect.h"

SaturationEffect::SaturationEffect()
{
    waveshaper.functionToUse = saturationFunction;
    updateParameters();
}

void SaturationEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    inputGain.prepare(spec);
    waveshaper.prepare(spec);
    outputGain.prepare(spec);
    updateParameters();
}

void SaturationEffect::process(juce::AudioBuffer<float>& buffer)
{
    if (!isEnabled)
        return;

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    // 入力ゲイン
    inputGain.process(context);

    // 原音のコピーを保存
    juce::AudioBuffer<float> dryBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        dryBuffer.copyFrom(ch, 0, buffer, ch, 0, buffer.getNumSamples());

    // サチュレーション処理
    waveshaper.process(context);
    outputGain.process(context);

    // ウェット/ドライミックス
    for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
    {
        float* wet = buffer.getWritePointer(ch);
        const float* dry = dryBuffer.getReadPointer(ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
            wet[i] = dry[i] * (1.0f - wetDry) + wet[i] * wetDry;
    }
}

void SaturationEffect::reset()
{
    inputGain.reset();
    waveshaper.reset();
    outputGain.reset();
}

float SaturationEffect::saturationFunction(float x)
{
    // タンジェントサチュレーション
    return std::tanh(x);
}

void SaturationEffect::updateParameters()
{
    const float driveGain = juce::Decibels::decibelsToGain(drive * 24.0f); // 最大24dBのドライブ
    inputGain.setGainLinear(driveGain);
    outputGain.setGainLinear(1.0f / driveGain); // メイクアップゲイン
}

juce::StringArray SaturationEffect::getParameterNames() const
{
    return {"Drive", "Mix"};
}

juce::StringArray SaturationEffect::getParameterLabels() const
{
    return {"dB", "%"};
}

juce::Array<float> SaturationEffect::getParameterRanges() const
{
    return {
        0.0f, 1.0f, 0.5f,  // Drive: 0-24dB
        0.0f, 1.0f, 0.5f   // Mix: 0-100%
    };
}

void SaturationEffect::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0:
            drive = value;
            updateParameters();
            break;
        case 1:
            wetDry = value;
            break;
        default:
            return;
    }
}

float SaturationEffect::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return drive;
        case 1: return wetDry;
        default: return 0.0f;
    }
}

void SaturationEffect::saveToXml(juce::XmlElement& xml) const
{
    xml.setAttribute("Drive", drive);
    xml.setAttribute("WetDry", wetDry);
}

void SaturationEffect::loadFromXml(const juce::XmlElement& xml)
{
    drive = static_cast<float>(xml.getDoubleAttribute("Drive", drive));
    wetDry = static_cast<float>(xml.getDoubleAttribute("WetDry", wetDry));
    updateParameters();
} 