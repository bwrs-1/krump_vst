#include "ReverbEffect.h"

ReverbEffect::~ReverbEffect()
{
}

void ReverbEffect::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
    updateParameters();
}

void ReverbEffect::processBlock(juce::AudioBuffer<float>& buffer)
{
    reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
}

void ReverbEffect::reset()
{
    reverb.reset();
}

void ReverbEffect::setRoomSize(float value)
{
    parameters.roomSize = value;
    updateParameters();
}

void ReverbEffect::setDamping(float value)
{
    parameters.damping = value;
    updateParameters();
}

void ReverbEffect::setWetLevel(float value)
{
    parameters.wetLevel = value;
    updateParameters();
}

void ReverbEffect::setDryLevel(float value)
{
    parameters.dryLevel = value;
    updateParameters();
}

void ReverbEffect::setWidth(float value)
{
    parameters.width = value;
    updateParameters();
}

void ReverbEffect::setFreezeMode(bool value)
{
    parameters.freezeMode = value ? 1.0f : 0.0f;
    updateParameters();
}

juce::StringArray ReverbEffect::getParameterNames() const
{
    return {"Room Size", "Damping", "Wet Level", "Dry Level", "Width", "Freeze"};
}

juce::StringArray ReverbEffect::getParameterLabels() const
{
    return {"%", "%", "%", "%", "%", ""};
}

juce::Array<float> ReverbEffect::getParameterRanges() const
{
    return {0.0f, 1.0f, 0.5f};  // min, max, default
}

void ReverbEffect::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0: setRoomSize(value); break;
        case 1: setDamping(value); break;
        case 2: setWetLevel(value); break;
        case 3: setDryLevel(value); break;
        case 4: setWidth(value); break;
        case 5: setFreezeMode(value > 0.5f); break;
        default: break;
    }
}

float ReverbEffect::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return parameters.roomSize;
        case 1: return parameters.damping;
        case 2: return parameters.wetLevel;
        case 3: return parameters.dryLevel;
        case 4: return parameters.width;
        case 5: return parameters.freezeMode;
        default: return 0.0f;
    }
}

void ReverbEffect::updateParameters()
{
    reverb.setParameters(parameters);
}

void ReverbEffect::saveToXml(juce::XmlElement& xml) const
{
    xml.setAttribute("RoomSize", parameters.roomSize);
    xml.setAttribute("Damping", parameters.damping);
    xml.setAttribute("WetLevel", parameters.wetLevel);
    xml.setAttribute("DryLevel", parameters.dryLevel);
    xml.setAttribute("Width", parameters.width);
    xml.setAttribute("FreezeMode", parameters.freezeMode > 0.5f ? 1 : 0);
}

void ReverbEffect::loadFromXml(const juce::XmlElement& xml)
{
    parameters.roomSize = static_cast<float>(xml.getDoubleAttribute("RoomSize", parameters.roomSize));
    parameters.damping = static_cast<float>(xml.getDoubleAttribute("Damping", parameters.damping));
    parameters.wetLevel = static_cast<float>(xml.getDoubleAttribute("WetLevel", parameters.wetLevel));
    parameters.dryLevel = static_cast<float>(xml.getDoubleAttribute("DryLevel", parameters.dryLevel));
    parameters.width = static_cast<float>(xml.getDoubleAttribute("Width", parameters.width));
    parameters.freezeMode = xml.getBoolAttribute("FreezeMode", parameters.freezeMode > 0.5f) ? 1.0f : 0.0f;
    updateParameters();
} 