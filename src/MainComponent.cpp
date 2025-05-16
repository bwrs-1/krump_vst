#include "MainComponent.h"

MainComponent::MainComponent(AudioPluginAudioProcessor& p)
    : audioProcessor(p)
{
    setupPads();
    setupKnobs();
    setupLCD();
    
    // コンポーネントのサイズを設定
    setSize(800, 600);
}

MainComponent::~MainComponent()
{
}

void MainComponent::paint(juce::Graphics& g)
{
    // 背景を塗りつぶし
    g.fillAll(backgroundColour);

    // パッドの描画
    for (const auto& pad : pads)
    {
        auto bounds = pad->getBounds();
        g.setColour(padColour);
        g.fillRoundedRectangle(bounds.toFloat(), 10.0f);
        
        g.setColour(textColour);
        g.drawRoundedRectangle(bounds.toFloat(), 10.0f, 2.0f);
    }

    // LCD画面の背景
    auto lcdBounds = lcdScreen->getBounds();
    g.setColour(juce::Colours::black);
    g.fillRect(lcdBounds);
    g.setColour(textColour);
    g.drawRect(lcdBounds, 1);
}

void MainComponent::resized()
{
    updateLayout();
}

void MainComponent::setupPads()
{
    // 8つのパッドを作成
    for (int i = 0; i < 8; ++i)
    {
        auto pad = std::make_unique<juce::TextButton>(juce::String("Pad ") + juce::String(i + 1));
        pad->setButtonText(juce::String(i + 1));
        addAndMakeVisible(pad.get());
        pads[i] = std::move(pad);
    }
}

void MainComponent::setupKnobs()
{
    // 3つのコントロールノブを作成
    for (int i = 0; i < 3; ++i)
    {
        auto knob = std::make_unique<juce::Slider>();
        knob->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        knob->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        knob->setRange(0.0, 100.0, 1.0);
        addAndMakeVisible(knob.get());
        controlKnobs[i] = std::move(knob);
    }

    // バリューノブを作成
    valueKnob = std::make_unique<juce::Slider>();
    valueKnob->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    valueKnob->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    valueKnob->setRange(0.0, 100.0, 1.0);
    addAndMakeVisible(valueKnob.get());
}

void MainComponent::setupLCD()
{
    lcdScreen = std::make_unique<juce::Label>();
    lcdScreen->setFont(juce::Font(24.0f, juce::Font::bold));
    lcdScreen->setColour(juce::Label::textColourId, textColour);
    lcdScreen->setJustificationType(juce::Justification::centred);
    lcdScreen->setText("TIME STRETCH: 45%", juce::dontSendNotification);
    addAndMakeVisible(lcdScreen.get());
}

void MainComponent::updateLayout()
{
    auto area = getLocalBounds();
    
    // LCD画面のレイアウト
    auto lcdArea = area.removeFromTop(80);
    lcdScreen->setBounds(lcdArea.reduced(10));

    // ノブのレイアウト
    auto knobArea = area.removeFromTop(200);
    auto knobWidth = knobArea.getWidth() / 4;
    
    for (int i = 0; i < 3; ++i)
    {
        controlKnobs[i]->setBounds(knobArea.removeFromLeft(knobWidth).reduced(10));
    }
    valueKnob->setBounds(knobArea.reduced(10));

    // パッドのレイアウト
    auto padArea = area;
    auto padWidth = padArea.getWidth() / 4;
    auto padHeight = padArea.getHeight() / 2;

    for (int row = 0; row < 2; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            int index = row * 4 + col;
            pads[index]->setBounds(col * padWidth, row * padHeight + padArea.getY(),
                                 padWidth, padHeight);
        }
    }
} 