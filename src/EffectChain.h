#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "audio/effects/Effect.h"

class EffectChain
{
public:
    EffectChain();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void addEffect(std::unique_ptr<Effect> effect);
    void removeEffect(int index);
    void moveEffect(int fromIndex, int toIndex);
    
    Effect* getEffect(int index);
    int getNumEffects() const;

    // XML関連
    void saveToXml(juce::XmlElement& xml) const;
    void loadFromXml(const juce::XmlElement& xml);

private:
    std::vector<std::unique_ptr<Effect>> effects;
    juce::dsp::ProcessSpec currentSpec;
}; 