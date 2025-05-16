#include <juce_audio_formats/juce_audio_formats.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

constexpr int numKnobs = 12;

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(p), processorRef(p)
{
    // 背景画像の読み込み
    backgroundImage = juce::ImageCache::getFromMemory(BinaryData::sp404_base_jpg, BinaryData::sp404_base_jpgSize);
    
    // ノブ画像の読み込み
    knobImage = juce::ImageCache::getFromMemory(BinaryData::knob_jpg, BinaryData::knob_jpgSize);

    // ノブスライダーを追加・初期化
    for (int i = 0; i < numKnobs; ++i)
    {
        knobSliders[i].setSliderStyle(juce::Slider::Rotary);
        knobSliders[i].setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        knobSliders[i].setLookAndFeel(&customLookAndFeel);
        addAndMakeVisible(knobSliders[i]);
    }

    // フィルターノブの設定
    filterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterCutoffSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "filterCutoff", filterCutoffSlider);

    filterResonanceSlider.setSliderStyle(juce::Slider::Rotary);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    filterResonanceSlider.setLookAndFeel(&customLookAndFeel);
    addAndMakeVisible(filterResonanceSlider);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "filterResonance", filterResonanceSlider);

    setSize(800, 400);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    for (auto& slider : knobSliders)
        slider.setLookAndFeel(nullptr);
    filterCutoffSlider.setLookAndFeel(nullptr);
    filterResonanceSlider.setLookAndFeel(nullptr);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    // 背景画像の描画
    g.drawImage(backgroundImage, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);

    // タイトルの描画
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font("Arial", 24.0f, juce::Font::bold));
    g.drawFittedText("BUCK-404mkII", 0, 10, getWidth(), 30, juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    const int rows = 3;
    const int cols = 4;
    const int knobGap = 20;
    const int knobSize = 60;

    const int totalKnobWidth = cols * knobSize + (cols - 1) * knobGap;
    const int totalKnobHeight = rows * knobSize + (rows - 1) * knobGap;

    const int startX = (getWidth() - totalKnobWidth) / 2;
    const int startY = (getHeight() - totalKnobHeight) / 2;

    for (int i = 0; i < numKnobs; ++i)
    {
        int row = i / cols;
        int col = i % cols;

        int x = startX + col * (knobSize + knobGap);
        int y = startY + row * (knobSize + knobGap);

        knobSliders[i].setBounds(x, y, knobSize, knobSize);
    }

    // フィルターノブの配置
    filterCutoffSlider.setBounds(startX - knobSize - knobGap, startY, knobSize, knobSize);
    filterResonanceSlider.setBounds(startX - knobSize - knobGap, startY + knobSize + knobGap, knobSize, knobSize);
}