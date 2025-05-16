#include "EffectChain.h"
#include "audio/effects/DelayEffect.h"
#include "audio/effects/FilterEffect.h"
#include "audio/effects/DistortionEffect.h"

EffectChain::EffectChain() = default;

void EffectChain::prepare(const juce::dsp::ProcessSpec& spec)
{
    currentSpec = spec;
    for (auto& effect : effects)
    {
        effect->prepare(spec);
    }
}

void EffectChain::process(juce::AudioBuffer<float>& buffer)
{
    for (auto& effect : effects)
    {
        effect->process(buffer);
    }
}

void EffectChain::reset()
{
    for (auto& effect : effects)
    {
        effect->reset();
    }
}

void EffectChain::addEffect(std::unique_ptr<Effect> effect)
{
    if (effect)
    {
        effect->prepare(currentSpec);
        effects.push_back(std::move(effect));
    }
}

void EffectChain::removeEffect(int index)
{
    if (index >= 0 && index < effects.size())
    {
        effects.erase(effects.begin() + index);
    }
}

void EffectChain::moveEffect(int fromIndex, int toIndex)
{
    if (fromIndex >= 0 && fromIndex < effects.size() &&
        toIndex >= 0 && toIndex < effects.size() &&
        fromIndex != toIndex)
    {
        auto effect = std::move(effects[fromIndex]);
        effects.erase(effects.begin() + fromIndex);
        effects.insert(effects.begin() + toIndex, std::move(effect));
    }
}

Effect* EffectChain::getEffect(int index)
{
    if (index >= 0 && index < effects.size())
    {
        return effects[index].get();
    }
    return nullptr;
}

int EffectChain::getNumEffects() const
{
    return static_cast<int>(effects.size());
}

void EffectChain::saveToXml(juce::XmlElement& xml) const
{
    for (size_t i = 0; i < effects.size(); ++i)
    {
        auto effectElement = xml.createNewChildElement("Effect");
        effectElement->setAttribute("Index", static_cast<int>(i));
        effectElement->setAttribute("Type", effects[i]->getName());
        effects[i]->saveToXml(*effectElement);
    }
}

void EffectChain::loadFromXml(const juce::XmlElement& xml)
{
    effects.clear();
    
    forEachXmlChildElementWithTagName(xml, effectElement, "Effect")
    {
        const juce::String type = effectElement->getStringAttribute("Type");
        std::unique_ptr<Effect> effect;
        
        // エフェクトの再作成
        if (type == "Delay")
            effect = std::make_unique<DelayEffect>();
        else if (type == "Filter")
            effect = std::make_unique<FilterEffect>();
        else if (type == "Distortion")
            effect = std::make_unique<DistortionEffect>();
            
        if (effect)
        {
            effect->loadFromXml(*effectElement);
            effect->prepare(currentSpec);
            effects.push_back(std::move(effect));
        }
    }
} 