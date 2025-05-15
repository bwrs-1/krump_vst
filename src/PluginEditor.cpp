#include <juce_audio_formats/juce_audio_formats.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

constexpr int numKnobs = 12;

//==============================================================================
// エディタのコンストラクタ
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(p), processorRef(p)
{
    // ノブスライダーを追加・初期化
    for (int i = 0; i < numKnobs; ++i)
    {
        knobSliders[i].setSliderStyle(juce::Slider::Rotary);
        knobSliders[i].setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
        addAndMakeVisible(knobSliders[i]);
    }

    addAndMakeVisible(filterCutoffSlider);
    filterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "filterCutoff", filterCutoffSlider);

    addAndMakeVisible(filterResonanceSlider);
    filterResonanceSlider.setSliderStyle(juce::Slider::Rotary);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "filterResonance", filterResonanceSlider);

    setSize(500, 400);
}

// デストラクタ
AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    // 特に明示的なクリーンアップは不要
}

//==============================================================================
// 画面描画
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("BUCK-404mkII", 0, 0, getWidth(), 30, juce::Justification::centred, 1);

    // 背景枠（ノブを囲む）
    g.setColour(juce::Colours::darkgrey);
    int knobSize = 80;
    int knobGap = 10;
    int cols = 4, rows = 3;
    int totalKnobWidth = cols * knobSize + (cols - 1) * knobGap;
    int totalKnobHeight = rows * knobSize + (rows - 1) * knobGap;
    int startX = (getWidth() - totalKnobWidth) / 2;
    int startY = (getHeight() - totalKnobHeight) / 2;
    g.drawRect(startX - 10, startY - 10, totalKnobWidth + 20, totalKnobHeight + 20, 2);
}

// レイアウト更新
void AudioPluginAudioProcessorEditor::resized()
{
    const int rows = 3;
    const int cols = 4;
    const int knobGap = 10;
    const int knobSize = 80;

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
}

// 配列宣言
juce::Slider knobSliders[numKnobs];
