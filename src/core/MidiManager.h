#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "../audio/effects/Effect.h"

class MidiManager
{
public:
    MidiManager();
    ~MidiManager();

    // MIDIマッピング
    void addMapping(int effectIndex, int parameterIndex, int ccNumber);
    void removeMapping(int effectIndex, int parameterIndex);
    void clearAllMappings();
    
    // MIDI処理
    void handleMidiMessage(const juce::MidiMessage& message);
    void handleMidiLearn(int effectIndex, int parameterIndex);
    void cancelMidiLearn();
    
    // マッピング状態
    bool isMidiLearnActive() const;
    bool isParameterMapped(int effectIndex, int parameterIndex) const;
    int getMappedCC(int effectIndex, int parameterIndex) const;
    
    // XMLシリアライズ
    void saveToXml(juce::XmlElement& xml) const;
    void loadFromXml(const juce::XmlElement& xml);

private:
    struct MidiMapping
    {
        int effectIndex;
        int parameterIndex;
        int ccNumber;
    };

    std::vector<MidiMapping> mappings;
    bool midiLearnMode;
    int learnEffectIndex;
    int learnParameterIndex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiManager)
}; 