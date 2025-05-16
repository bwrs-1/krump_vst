#include "MainComponent.h"
#include "../../PluginProcessor.h"
#include "../../audio/effects/DelayEffect.h"
#include "../../audio/effects/FilterEffect.h"
#include "../../audio/effects/DistortionEffect.h"

MainComponent::MainComponent(AudioPluginAudioProcessor& processor)
    : audioProcessor(processor),
      presetManager(std::make_unique<PresetManager>(processor.effectChain))
{
    // ディスプレイの設定
    display = std::make_unique<DisplayLabel>("SP-404MKII Clone");
    addAndMakeVisible(display.get());

    // コントロールの初期化
    setupKnobs();
    setupButtons();
    setupPads();

    // プリセットリストの更新
    updatePresetList();
}

MainComponent::~MainComponent()
{
    // クリーンアップ処理
}

void MainComponent::paint(juce::Graphics& g)
{
    // 背景を描画
    g.fillAll(SP404LookAndFeel::background);
}

void MainComponent::resized()
{
    auto bounds = getLocalBounds();
    
    // ディスプレイの配置
    display->setBounds(getScaledBounds(0.1f, 0.05f, 0.8f, 0.1f));

    // エフェクトボタンの配置
    for (size_t i = 0; i < effectButtons.size(); ++i)
    {
        float x = 0.1f + (i % 3) * 0.3f;
        float y = 0.2f;
        effectButtons[i]->setBounds(getScaledBounds(x, y, 0.2f, 0.1f));
    }

    // ノブの配置
    for (size_t i = 0; i < controlKnobs.size(); ++i)
    {
        float x = 0.1f + (i % 3) * 0.3f;
        float y = 0.35f + (i / 3) * 0.2f;
        controlKnobs[i]->setBounds(getScaledBounds(x, y, 0.2f, 0.15f));
    }

    // プリセットボタンの配置
    for (size_t i = 0; i < presetButtons.size(); ++i)
    {
        float x = 0.1f + (i % 3) * 0.3f;
        float y = 0.75f;
        presetButtons[i]->setBounds(getScaledBounds(x, y, 0.2f, 0.1f));
    }

    // パッドの配置
    const int padRows = 4;
    const int padCols = 4;
    for (int row = 0; row < padRows; ++row)
    {
        for (int col = 0; col < padCols; ++col)
        {
            const int index = row * padCols + col;
            float x = 0.1f + col * 0.2f;
            float y = 0.9f + row * 0.15f;
            pads[index]->setBounds(getScaledBounds(x, y, 0.15f, 0.1f));
        }
    }
}

void MainComponent::buttonClicked(juce::Button* button)
{
    // エフェクトボタンの処理
    for (size_t i = 0; i < effectButtons.size(); ++i)
    {
        if (button == effectButtons[i].get())
        {
            audioProcessor.effectChain.getEffect(static_cast<int>(i))->setEnabled(effectButtons[i]->getToggleState());
            updateDisplay();
            return;
        }
    }

    // プリセットボタンの処理
    for (size_t i = 0; i < presetButtons.size(); ++i)
    {
        if (button == presetButtons[i].get())
        {
            if (button->getToggleState())
            {
                presetManager->loadPreset(static_cast<int>(i));
            }
            updateDisplay();
            return;
        }
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    // ノブの処理
    for (size_t i = 0; i < controlKnobs.size(); ++i)
    {
        if (slider == controlKnobs[i].get())
        {
            auto* effect = audioProcessor.effectChain.getEffect(static_cast<int>(i / 2));
            if (effect)
            {
                effect->setParameter(static_cast<int>(i % 2), slider->getValue());
                updateDisplay();
            }
            return;
        }
    }
}

juce::Rectangle<int> MainComponent::getScaledBounds(float x, float y, float w, float h) const
{
    auto bounds = getLocalBounds();
    return juce::Rectangle<int>(
        static_cast<int>(bounds.getWidth() * x),
        static_cast<int>(bounds.getHeight() * y),
        static_cast<int>(bounds.getWidth() * w),
        static_cast<int>(bounds.getHeight() * h)
    );
}

void MainComponent::setupKnobs()
{
    const juce::StringArray knobNames = {
        "Size", "Damping",           // Reverb
        "Time", "Feedback",          // Delay
        "Cutoff", "Resonance",       // Filter
        "Pitch", "Fine",             // Pitch
        "Drive", "Mix"               // Saturation
    };

    for (int i = 0; i < 10; ++i)
    {
        auto knob = std::make_unique<RotaryKnob>(knobNames[i]);
        knob->setRange(0.0, 1.0);
        knob->setValue(0.5);
        knob->addListener(this);
        addAndMakeVisible(knob.get());
        controlKnobs.push_back(std::move(knob));
    }
}

void MainComponent::setupButtons()
{
    // エフェクトボタン
    const juce::StringArray effectNames = {
        "Reverb", "Delay", "Filter", "Pitch", "Drive"
    };

    for (const auto& name : effectNames)
    {
        auto button = std::make_unique<EffectButton>(name);
        button->addListener(this);
        addAndMakeVisible(button.get());
        effectButtons.push_back(std::move(button));
    }

    // プリセットボタン
    for (int i = 1; i <= 3; ++i)
    {
        auto button = std::make_unique<PresetButton>("Preset");
        button->setPresetNumber(i);
        button->addListener(this);
        addAndMakeVisible(button.get());
        presetButtons.push_back(std::move(button));
    }
}

void MainComponent::setupPads()
{
    for (int i = 0; i < 16; ++i)
    {
        auto pad = std::make_unique<juce::TextButton>(juce::String(i + 1));
        pad->setButtonText(juce::String(i + 1));
        pad->addListener(this);
        addAndMakeVisible(pad.get());
        pads.push_back(std::move(pad));
    }
}

void MainComponent::updateDisplay()
{
    // アクティブなエフェクトの状態を表示
    juce::String displayText;
    for (size_t i = 0; i < effectButtons.size(); ++i)
    {
        if (effectButtons[i]->getToggleState())
        {
            if (!displayText.isEmpty())
                displayText += " + ";
            displayText += effectButtons[i]->getButtonText();
        }
    }

    if (displayText.isEmpty())
        displayText = "Ready";

    display->setText(displayText, juce::dontSendNotification);
}

void MainComponent::updatePresetList()
{
    // プリセットの状態を更新
    for (size_t i = 0; i < presetButtons.size(); ++i)
    {
        presetButtons[i]->setEnabled(presetManager->presetExists(static_cast<int>(i)));
    }
} 