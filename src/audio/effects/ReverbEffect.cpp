#include "ReverbEffect.h"

ReverbEffect::ReverbEffect()
{
    updateReverbParameters();
}

void ReverbEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    reverb.prepare(spec);
    updateReverbParameters();
}

void ReverbEffect::process(juce::AudioBuffer<float>& buffer)
{
    if (!isEnabled)
        return;

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    reverb.process(context);
}

void ReverbEffect::reset()
{
    reverb.reset();
}

void ReverbEffect::updateReverbParameters()
{
    juce::dsp::Reverb::Parameters params;
    params.roomSize = roomSize;
    params.damping = damping;
    params.width = 0.5f;
    params.wetLevel = 0.33f;
    params.dryLevel = 0.4f;
    params.freezeMode = 0.0f;
    reverb.setParameters(params);
}

juce::StringArray ReverbEffect::getParameterNames() const
{
    return {"Size", "Damping"};
}

juce::StringArray ReverbEffect::getParameterLabels() const
{
    return {"%", "%"};
}

juce::Array<float> ReverbEffect::getParameterRanges() const
{
    return {
        0.0f, 1.0f, 0.5f,  // Size: 0-100%
        0.0f, 1.0f, 0.5f   // Damping: 0-100%
    };
}

void ReverbEffect::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0:
            roomSize = value;
            break;
        case 1:
            damping = value;
            break;
        default:
            return;
    }
    updateReverbParameters();
}

float ReverbEffect::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return roomSize;
        case 1: return damping;
        default: return 0.0f;
    }
}

void ReverbEffect::saveToXml(juce::XmlElement& xml) const
{
    xml.setAttribute("RoomSize", roomSize);
    xml.setAttribute("Damping", damping);
}

void ReverbEffect::loadFromXml(const juce::XmlElement& xml)
{
    roomSize = static_cast<float>(xml.getDoubleAttribute("RoomSize", roomSize));
    damping = static_cast<float>(xml.getDoubleAttribute("Damping", damping));
    updateReverbParameters();
} 