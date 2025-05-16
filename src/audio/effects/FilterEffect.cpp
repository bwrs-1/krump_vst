#include "FilterEffect.h"

FilterEffect::FilterEffect()
{
    filter.setType(juce::dsp::StateVariableTPTFilterType::lowpass);
    updateFilterParameters();
}

void FilterEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    filter.prepare(spec);
    updateFilterParameters();
}

void FilterEffect::process(juce::AudioBuffer<float>& buffer)
{
    if (!isEnabled)
        return;

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);
    filter.process(context);
}

void FilterEffect::reset()
{
    filter.reset();
}

void FilterEffect::updateFilterParameters()
{
    filter.setCutoffFrequency(cutoff);
    filter.setResonance(resonance);
}

juce::StringArray FilterEffect::getParameterNames() const
{
    return {"Cutoff", "Resonance"};
}

juce::StringArray FilterEffect::getParameterLabels() const
{
    return {"Hz", "Q"};
}

juce::Array<float> FilterEffect::getParameterRanges() const
{
    return {
        20.0f, 20000.0f, 1000.0f,  // Cutoff: 20Hz - 20kHz
        0.1f, 8.0f, 0.7f           // Resonance: 0.1 - 8.0
    };
}

void FilterEffect::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0:
            cutoff = value;
            break;
        case 1:
            resonance = value;
            break;
        default:
            return;
    }
    updateFilterParameters();
}

float FilterEffect::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return cutoff;
        case 1: return resonance;
        default: return 0.0f;
    }
}

void FilterEffect::saveToXml(juce::XmlElement& xml) const
{
    xml.setAttribute("Cutoff", cutoff);
    xml.setAttribute("Resonance", resonance);
}

void FilterEffect::loadFromXml(const juce::XmlElement& xml)
{
    cutoff = static_cast<float>(xml.getDoubleAttribute("Cutoff", cutoff));
    resonance = static_cast<float>(xml.getDoubleAttribute("Resonance", resonance));
    updateFilterParameters();
} 