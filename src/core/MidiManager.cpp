#include "MidiManager.h"

MidiManager::MidiManager() : midiLearnMode(false), learnEffectIndex(-1), learnParameterIndex(-1)
{
}

MidiManager::~MidiManager() = default;

void MidiManager::addMapping(int effectIndex, int parameterIndex, int ccNumber)
{
    // 既存のマッピングを確認
    removeMapping(effectIndex, parameterIndex);
    
    // 新しいマッピングを追加
    MidiMapping mapping{effectIndex, parameterIndex, ccNumber};
    mappings.push_back(mapping);
}

void MidiManager::removeMapping(int effectIndex, int parameterIndex)
{
    mappings.erase(
        std::remove_if(mappings.begin(), mappings.end(),
            [effectIndex, parameterIndex](const MidiMapping& mapping)
            {
                return mapping.effectIndex == effectIndex &&
                       mapping.parameterIndex == parameterIndex;
            }),
        mappings.end()
    );
}

void MidiManager::clearAllMappings()
{
    mappings.clear();
}

void MidiManager::handleMidiMessage(const juce::MidiMessage& message)
{
    if (message.isController())
    {
        const int ccNumber = message.getControllerNumber();
        const float value = message.getControllerValue() / 127.0f;

        if (midiLearnMode && learnEffectIndex >= 0 && learnParameterIndex >= 0)
        {
            // MIDIラーンモード中の場合、新しいマッピングを作成
            addMapping(learnEffectIndex, learnParameterIndex, ccNumber);
            cancelMidiLearn();
        }
        else
        {
            // 通常のMIDI処理
            for (const auto& mapping : mappings)
            {
                if (mapping.ccNumber == ccNumber)
                {
                    // エフェクトのパラメータを更新
                    // Note: この部分は実際のエフェクトチェーンの実装に依存
                }
            }
        }
    }
}

void MidiManager::handleMidiLearn(int effectIndex, int parameterIndex)
{
    midiLearnMode = true;
    learnEffectIndex = effectIndex;
    learnParameterIndex = parameterIndex;
}

void MidiManager::cancelMidiLearn()
{
    midiLearnMode = false;
    learnEffectIndex = -1;
    learnParameterIndex = -1;
}

bool MidiManager::isMidiLearnActive() const
{
    return midiLearnMode;
}

bool MidiManager::isParameterMapped(int effectIndex, int parameterIndex) const
{
    return std::any_of(mappings.begin(), mappings.end(),
        [effectIndex, parameterIndex](const MidiMapping& mapping)
        {
            return mapping.effectIndex == effectIndex &&
                   mapping.parameterIndex == parameterIndex;
        });
}

int MidiManager::getMappedCC(int effectIndex, int parameterIndex) const
{
    auto it = std::find_if(mappings.begin(), mappings.end(),
        [effectIndex, parameterIndex](const MidiMapping& mapping)
        {
            return mapping.effectIndex == effectIndex &&
                   mapping.parameterIndex == parameterIndex;
        });

    return it != mappings.end() ? it->ccNumber : -1;
}

void MidiManager::saveToXml(juce::XmlElement& xml) const
{
    for (const auto& mapping : mappings)
    {
        auto* mappingElement = new juce::XmlElement("MidiMapping");
        mappingElement->setAttribute("effectIndex", mapping.effectIndex);
        mappingElement->setAttribute("parameterIndex", mapping.parameterIndex);
        mappingElement->setAttribute("ccNumber", mapping.ccNumber);
        xml.addChildElement(mappingElement);
    }
}

void MidiManager::loadFromXml(const juce::XmlElement& xml)
{
    clearAllMappings();

    forEachXmlChildElementWithTagName(xml, mappingElement, "MidiMapping")
    {
        const int effectIndex = mappingElement->getIntAttribute("effectIndex");
        const int parameterIndex = mappingElement->getIntAttribute("parameterIndex");
        const int ccNumber = mappingElement->getIntAttribute("ccNumber");
        addMapping(effectIndex, parameterIndex, ccNumber);
    }
} 