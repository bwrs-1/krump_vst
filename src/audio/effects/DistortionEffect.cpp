#include "DistortionEffect.h"

DistortionEffect::DistortionEffect()
{
    waveshaper.functionToUse = distortionFunction;
    updateParameters();
}

void DistortionEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    inputGain.prepare(spec);
    waveshaper.prepare(spec);
    outputGain.prepare(spec);
    updateParameters();
}

void DistortionEffect::process(juce::AudioBuffer<float>& buffer)
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

    // ディストーション処理
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

void DistortionEffect::reset()
{
    inputGain.reset();
    waveshaper.reset();
    outputGain.reset();
}

float DistortionEffect::distortionFunction(float x)
{
    // タンジェントディストーション
    return std::tanh(x * 3.0f);
}

void DistortionEffect::updateParameters()
{
    const float driveGain = juce::Decibels::decibelsToGain(drive * 36.0f); // 最大36dBのドライブ
    inputGain.setGainLinear(driveGain);
    outputGain.setGainLinear(1.0f / driveGain); // メイクアップゲイン
}

juce::StringArray DistortionEffect::getParameterNames() const
{
    return {"Drive", "Mix"};
}

juce::StringArray DistortionEffect::getParameterLabels() const
{
    return {"dB", "%"};
}

juce::Array<float> DistortionEffect::getParameterRanges() const
{
    return {
        0.0f, 1.0f, 0.5f,  // Drive: 0-36dB
        0.0f, 1.0f, 0.5f   // Mix: 0-100%
    };
}

void DistortionEffect::setParameter(int parameterIndex, float value)
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

float DistortionEffect::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return drive;
        case 1: return wetDry;
        default: return 0.0f;
    }
}

void DistortionEffect::saveToXml(juce::XmlElement& xml) const
{
    xml.setAttribute("Drive", drive);
    xml.setAttribute("WetDry", wetDry);
}

void DistortionEffect::loadFromXml(const juce::XmlElement& xml)
{
    drive = static_cast<float>(xml.getDoubleAttribute("Drive", drive));
    wetDry = static_cast<float>(xml.getDoubleAttribute("WetDry", wetDry));
    updateParameters();
} 