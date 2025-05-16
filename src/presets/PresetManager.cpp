#include "PresetManager.h"
#include "../PluginProcessor.h"

PresetManager::PresetManager(EffectChain& effectChainToUse)
    : effectChain(effectChainToUse)
{
    // プリセットディレクトリが存在しない場合は作成
    getPresetDirectory().createDirectory();
}

void PresetManager::savePreset(int index, const juce::String& name)
{
    // エフェクトチェーンの状態をXMLに保存
    auto state = std::make_unique<juce::XmlElement>("Preset");
    state->setAttribute("Name", name);
    effectChain.saveToXml(*state);

    // プリセットを保存
    Preset preset{name, std::move(state)};
    presets[index] = std::move(preset);

    // ファイルに保存
    auto file = getPresetFile(index);
    if (auto* xml = presets[index].state.get())
        xml->writeTo(file, {});
}

void PresetManager::loadPreset(int index)
{
    if (!presetExists(index))
        return;

    // ファイルからプリセットを読み込み
    auto file = getPresetFile(index);
    if (auto xml = juce::XmlDocument::parse(file))
    {
        effectChain.loadFromXml(*xml);
        presets[index].state = std::move(xml);
    }
}

void PresetManager::deletePreset(int index)
{
    if (presetExists(index))
    {
        presets.erase(index);
        getPresetFile(index).deleteFile();
    }
}

bool PresetManager::presetExists(int index) const
{
    return presets.find(index) != presets.end();
}

juce::String PresetManager::getPresetName(int index) const
{
    if (auto it = presets.find(index); it != presets.end())
        return it->second.name;
    return {};
}

int PresetManager::getNumPresets() const
{
    return static_cast<int>(presets.size());
}

void PresetManager::saveToXml(juce::XmlElement& xml) const
{
    auto* presetsXml = xml.createNewChildElement("Presets");
    for (const auto& [index, preset] : presets)
    {
        if (preset.state)
        {
            auto* presetXml = presetsXml->createNewChildElement("Preset");
            presetXml->setAttribute("Index", index);
            presetXml->setAttribute("Name", preset.name);
            presetXml->addChildElement(new juce::XmlElement(*preset.state));
        }
    }
}

void PresetManager::loadFromXml(const juce::XmlElement& xml)
{
    presets.clear();

    if (auto* presetsXml = xml.getChildByName("Presets"))
    {
        for (auto* presetXml : presetsXml->getChildIterator())
        {
            const int index = presetXml->getIntAttribute("Index");
            const juce::String name = presetXml->getStringAttribute("Name");
            
            if (auto* stateXml = presetXml->getFirstChildElement())
            {
                Preset preset;
                preset.name = name;
                preset.state = std::make_unique<juce::XmlElement>(*stateXml);
                presets[index] = std::move(preset);
            }
        }
    }
}

juce::File PresetManager::getPresetDirectory() const
{
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile("SP404MKIIClone")
        .getChildFile("Presets");
}

juce::File PresetManager::getPresetFile(int index) const
{
    return getPresetDirectory()
        .getChildFile("preset_" + juce::String(index) + ".xml");
} 