#include <juce_audio_formats/juce_audio_formats.h>
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// エディタのコンストラクタ
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (p), processorRef (p)
{
    // パッドボタンを追加・初期化
    for (int i = 0; i < numPads; ++i)
    {
        padButtons[i].setButtonText(juce::String(i + 1)); // ボタンに番号を表示
        addAndMakeVisible(padButtons[i]);
        padButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
        padButtons[i].setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        // サンプルロード機能
        padButtons[i].onClick = [this, i]() {
            juce::FileChooser chooser("サンプルを選択...", {}, "*.wav;*.aiff;*.mp3");
            chooser.launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [this, i](const juce::FileChooser& fc) {
                    auto file = fc.getResult();
                    if (file.existsAsFile()) {
                        juce::AudioFormatManager formatManager;
                        formatManager.registerBasicFormats();
                        std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
                        if (reader) {
                            // サンプルをpadSamplesに読み込む
                            processorRef.padSamples[i].setSize((int)reader->numChannels, (int)reader->lengthInSamples);
                            reader->read(&processorRef.padSamples[i], 0, (int)reader->lengthInSamples, 0, true, true);
                            processorRef.padSampleLoaded[i] = true;
                        }
                    }
                }
            );
        };

        // ボタン押下時の色変更
        padButtons[i].onStateChange = [this, i]() {
            if (padButtons[i].isDown())
                padButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::red);
            else
                padButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::orange);
        };
    }

    // フィルターカットオフスライダーの初期化・パラメータバインド
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffSlider.setSliderStyle(juce::Slider::Rotary);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "filterCutoff", filterCutoffSlider);

    // フィルターレゾナンススライダーの初期化・パラメータバインド
    addAndMakeVisible(filterResonanceSlider);
    filterResonanceSlider.setSliderStyle(juce::Slider::Rotary);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "filterResonance", filterResonanceSlider);

    setSize (400, 200); // ウィンドウサイズ
}

// デストラクタ
AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
// 画面描画
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black); // 背景を黒で塗りつぶす

    // タイトル描画
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("BUCK-404mkII", 0, 0, getWidth(), 40, juce::Justification::centred, 1);

    // パッドエリアの枠
    g.setColour(juce::Colours::darkgrey);
    g.drawRect(10, 50, 400, 260, 2);
}

// レイアウト更新
void AudioPluginAudioProcessorEditor::resized()
{
    // 4x4パッド配置（例）
    int padSize = 80, padGap = 10;
    for (int i = 0; i < numPads; ++i)
    {
        int row = i / 4, col = i % 4;
        padButtons[i].setBounds(20 + col * (padSize + padGap),
                                60 + row * (padSize + padGap),
                                padSize, padSize);
    }
    // フィルターつまみの配置
    filterCutoffSlider.setBounds(40, 40, 120, 120);
    filterResonanceSlider.setBounds(200, 40, 120, 120);
}
