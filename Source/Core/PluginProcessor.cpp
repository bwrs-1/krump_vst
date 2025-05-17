#include "PluginProcessor.h"
#include "../GUI/PluginEditor.h"
#include "../DSP/ReverbEffect.h"

KrumpVSTAudioProcessor::KrumpVSTAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

KrumpVSTAudioProcessor::~KrumpVSTAudioProcessor()
{
}

void KrumpVSTAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    reverbEffect.prepareToPlay(sampleRate, samplesPerBlock);
}

void KrumpVSTAudioProcessor::releaseResources()
{
    reverbEffect.reset();
}

void KrumpVSTAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    reverbEffect.setRoomSize(apvts.getRawParameterValue("RoomSize")->load());
    reverbEffect.setDamping(apvts.getRawParameterValue("Damping")->load());
    reverbEffect.setWetLevel(apvts.getRawParameterValue("Wet")->load());
    reverbEffect.setDryLevel(apvts.getRawParameterValue("Dry")->load());
    reverbEffect.setWidth(apvts.getRawParameterValue("Width")->load());
    reverbEffect.setFreezeMode(apvts.getRawParameterValue("Freeze")->load() > 0.5f);
    reverbEffect.processBlock(buffer);
    midiMessages.clear();
}

juce::AudioProcessorEditor* KrumpVSTAudioProcessor::createEditor()
{
    return new KrumpVSTAudioProcessorEditor(*this);
}

void KrumpVSTAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void KrumpVSTAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorValueTreeState::ParameterLayout KrumpVSTAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>("RoomSize", "Room Size", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Damping",  "Damping",  0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Wet",      "Wet",      0.0f, 1.0f, 0.33f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Dry",      "Dry",      0.0f, 1.0f, 0.4f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Width",    "Width",    0.0f, 1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("Freeze",   "Freeze",   0.0f, 1.0f, 0.0f));
    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new KrumpVSTAudioProcessor();
}