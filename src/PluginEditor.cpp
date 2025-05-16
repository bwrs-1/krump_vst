#include "PluginEditor.h"

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p), mainComponent(p)
{
    addAndMakeVisible(mainComponent);
    setSize(800, 600);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    // MainComponentが描画を処理するため、ここでは何もしない
}

void AudioPluginAudioProcessorEditor::resized()
{
    // MainComponentをエディター全体に広げる
    mainComponent.setBounds(getLocalBounds());
}