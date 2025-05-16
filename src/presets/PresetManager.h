#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "../EffectChain.h"

class PresetManager
{
public:
    PresetManager(EffectChain& effectChainToUse);
    ~PresetManager() = default;

    void savePreset(int index, const juce::String& name);
    void loadPreset(int index);
    void deletePreset(int index);

    bool presetExists(int index) const;
    juce::String getPresetName(int index) const;
    int getNumPresets() const;

    void saveToXml(juce::XmlElement& xml) const;
    void loadFromXml(const juce::XmlElement& xml);

private:
    struct Preset
    {
        juce::String name;
        std::unique_ptr<juce::XmlElement> state;
    };

    EffectChain& effectChain;
    std::map<int, Preset> presets;

    juce::File getPresetDirectory() const;
    juce::File getPresetFile(int index) const;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
}; 