#include "DelayEffect.h"

DelayEffect::DelayEffect()
{
    updateDelayTime();
}

void DelayEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    lastSampleRate = static_cast<float>(spec.sampleRate);
    delayBufferLength = static_cast<int>(2.0f * lastSampleRate); // 最大2秒のディレイ
    delayBuffer.setSize(spec.numChannels, delayBufferLength);
    delayBuffer.clear();
    writePosition = 0;
    updateDelayTime();
}

void DelayEffect::process(juce::AudioBuffer<float>& buffer)
{
    if (!isEnabled)
        return;

    const int numChannels = buffer.getNumChannels();
    const int bufferLength = buffer.getNumSamples();
    const float delayTime = time * lastSampleRate;

    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        float* delayData = delayBuffer.getWritePointer(channel);

        for (int sample = 0; sample < bufferLength; ++sample)
        {
            const float in = channelData[sample];
            float readPosition = writePosition - delayTime;
            
            if (readPosition < 0)
                readPosition += delayBufferLength;

            // 線形補間
            const int readPos1 = static_cast<int>(readPosition);
            const int readPos2 = (readPos1 + 1) % delayBufferLength;
            const float fraction = readPosition - readPos1;
            
            const float delay = delayData[readPos1] + fraction * (delayData[readPos2] - delayData[readPos1]);
            
            // ディレイ信号を更新
            delayData[writePosition] = in + (delay * feedback);
            
            // 出力信号を更新
            channelData[sample] = in + delay;

            writePosition = (writePosition + 1) % delayBufferLength;
        }
    }
}

void DelayEffect::reset()
{
    delayBuffer.clear();
    writePosition = 0;
}

void DelayEffect::updateDelayTime()
{
    // ディレイタイムを更新（最大2秒）
    time = juce::jlimit(0.0f, 2.0f, time);
}

juce::StringArray DelayEffect::getParameterNames() const
{
    return {"Time", "Feedback"};
}

juce::StringArray DelayEffect::getParameterLabels() const
{
    return {"sec", "%"};
}

juce::Array<float> DelayEffect::getParameterRanges() const
{
    return {
        0.0f, 2.0f, 0.5f,  // Time: 0-2秒
        0.0f, 0.95f, 0.5f  // Feedback: 0-95%
    };
}

void DelayEffect::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0:
            time = value;
            updateDelayTime();
            break;
        case 1:
            feedback = value;
            break;
        default:
            break;
    }
}

float DelayEffect::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return time;
        case 1: return feedback;
        default: return 0.0f;
    }
}

void DelayEffect::saveToXml(juce::XmlElement& xml) const
{
    xml.setAttribute("Time", time);
    xml.setAttribute("Feedback", feedback);
}

void DelayEffect::loadFromXml(const juce::XmlElement& xml)
{
    time = static_cast<float>(xml.getDoubleAttribute("Time", time));
    feedback = static_cast<float>(xml.getDoubleAttribute("Feedback", feedback));
    updateDelayTime();
} 