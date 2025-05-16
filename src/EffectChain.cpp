#include "EffectChain.h"

// DelayEffect implementation
DelayEffect::DelayEffect() {}

void DelayEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = static_cast<float>(spec.sampleRate);
    delayBufferLength = static_cast<int>(2.0 * sampleRate); // 最大2秒のディレイ
    delayBuffer.setSize(static_cast<int>(spec.numChannels), delayBufferLength);
    delayBuffer.clear();
    writePosition = 0;
}

void DelayEffect::process(juce::AudioBuffer<float>& buffer)
{
    if (!isEnabled)
        return;

    const int numChannels = buffer.getNumChannels();
    const int bufferLength = buffer.getNumSamples();
    const int delayInSamples = static_cast<int>(time * sampleRate);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* delayData = delayBuffer.getWritePointer(channel);

        for (int sample = 0; sample < bufferLength; ++sample)
        {
            const float in = channelData[sample];
            float out = 0.0f;

            int readPosition = writePosition - delayInSamples;
            if (readPosition < 0)
                readPosition += delayBufferLength;

            out = delayData[readPosition];
            delayData[writePosition] = in + (out * feedback);

            channelData[sample] = in * (1.0f - mix) + out * mix;

            if (++writePosition >= delayBufferLength)
                writePosition = 0;
        }
    }
}

void DelayEffect::reset()
{
    delayBuffer.clear();
    writePosition = 0;
}

juce::StringArray DelayEffect::getParameterNames() const
{
    return {"Time", "Feedback", "Mix"};
}

void DelayEffect::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0: time = value; break;
        case 1: feedback = value; break;
        case 2: mix = value; break;
    }
}

float DelayEffect::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return time;
        case 1: return feedback;
        case 2: return mix;
        default: return 0.0f;
    }
}

// FilterEffect implementation
FilterEffect::FilterEffect() {}

void FilterEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = static_cast<float>(spec.sampleRate);
    filter.prepare(spec);
    updateFilter();
    reset();
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

void FilterEffect::updateFilter()
{
    switch (type)
    {
        case 0: // LowPass
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass(
                sampleRate, frequency, resonance);
            break;
        case 1: // HighPass
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
                sampleRate, frequency, resonance);
            break;
        case 2: // BandPass
            *filter.state = *juce::dsp::IIR::Coefficients<float>::makeBandPass(
                sampleRate, frequency, resonance);
            break;
    }
}

juce::StringArray FilterEffect::getParameterNames() const
{
    return {"Frequency", "Resonance", "Type"};
}

void FilterEffect::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0:
            frequency = value;
            break;
        case 1:
            resonance = value;
            break;
        case 2:
            type = static_cast<int>(value);
            break;
    }
    updateFilter();
}

float FilterEffect::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return frequency;
        case 1: return resonance;
        case 2: return static_cast<float>(type);
        default: return 0.0f;
    }
}

// EffectChain implementation
EffectChain::EffectChain() {}

void EffectChain::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSpec = spec;
    for (auto& effect : effects)
        effect->prepare(spec);
}

void EffectChain::process(juce::AudioBuffer<float>& buffer)
{
    for (auto& effect : effects)
        effect->process(buffer);
}

void EffectChain::reset()
{
    for (auto& effect : effects)
        effect->reset();
}

void EffectChain::addEffect(std::unique_ptr<Effect> effect)
{
    effect->prepare(currentSpec);
    effects.push_back(std::move(effect));
}

void EffectChain::removeEffect(int index)
{
    if (index >= 0 && static_cast<size_t>(index) < effects.size())
        effects.erase(effects.begin() + index);
}

void EffectChain::moveEffect(int fromIndex, int toIndex)
{
    if (fromIndex >= 0 && static_cast<size_t>(fromIndex) < effects.size() &&
        toIndex >= 0 && static_cast<size_t>(toIndex) < effects.size())
    {
        auto effect = std::move(effects[static_cast<size_t>(fromIndex)]);
        effects.erase(effects.begin() + fromIndex);
        effects.insert(effects.begin() + toIndex, std::move(effect));
    }
}

Effect* EffectChain::getEffect(int index)
{
    if (index >= 0 && static_cast<size_t>(index) < effects.size())
        return effects[static_cast<size_t>(index)].get();
    return nullptr;
}

int EffectChain::getNumEffects() const
{
    return static_cast<int>(effects.size());
} 