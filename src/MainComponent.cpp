#include "MainComponent.h"

// SP404Pad implementation
void SP404Pad::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted,
                          bool shouldDrawButtonAsDown)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    auto cornerSize = 10.0f;
    
    // パッドの基本的な形状
    g.setColour(shouldDrawButtonAsDown ? juce::Colour(255, 159, 0) 
                                     : juce::Colour(60, 60, 60));
    g.fillRoundedRectangle(bounds, cornerSize);

    // 光沢効果
    if (!shouldDrawButtonAsDown)
    {
        juce::ColourGradient gloss(juce::Colours::white.withAlpha(0.3f),
                                  bounds.getTopLeft(),
                                  juce::Colours::transparentWhite,
                                  bounds.getBottomRight(),
                                  true);
        g.setGradientFill(gloss);
        g.fillRoundedRectangle(bounds, cornerSize);
    }

    // エッジライト効果
    if (shouldDrawButtonAsHighlighted)
    {
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.drawRoundedRectangle(bounds, cornerSize, 2.0f);
    }

    // テキスト
    g.setColour(shouldDrawButtonAsDown ? juce::Colours::black 
                                     : juce::Colours::white);
    g.setFont(16.0f);
    g.drawText(getName(), bounds, juce::Justification::centred);
}

// SP404Knob implementation
SP404Knob::SP404Knob(const juce::String& labelText)
    : juce::Slider(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag,
                   juce::Slider::TextEntryBoxPosition::NoTextBox),
      label(labelText)
{
    setRotaryParameters(juce::MathConstants<float>::pi * 1.2f,
                       juce::MathConstants<float>::pi * 2.8f,
                       true);
}

void SP404Knob::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    auto centre = bounds.getCentre();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.4f;

    // ノブの本体
    g.setColour(juce::Colour(40, 40, 40));
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    // 金属効果
    juce::ColourGradient metalGradient(juce::Colour(80, 80, 80),
                                      centre.withY(centre.y - radius),
                                      juce::Colour(30, 30, 30),
                                      centre.withY(centre.y + radius),
                                      true);
    g.setGradientFill(metalGradient);
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);

    // 回転インジケーター
    auto angle = (getValue() - getMinimum()) / (getMaximum() - getMinimum()) * 300.0f - 150.0f;
    auto angleRad = angle * (juce::MathConstants<float>::pi / 180.0f);
    
    juce::Path indicator;
    indicator.addRectangle(-2.0f, -radius * 0.8f, 4.0f, radius * 0.3f);
    
    g.setColour(juce::Colour(0, 172, 255));
    g.fillPath(indicator, juce::AffineTransform::rotation(angleRad)
                                                .translated(centre.x, centre.y));

    // ラベル
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);
    g.drawText(label, bounds, juce::Justification::centredBottom);
}

// SP404Display implementation
SP404Display::SP404Display()
    : displayText("KRUMP VST")
{
    displayBuffer = juce::Image(juce::Image::RGB, 400, 100, true);
}

void SP404Display::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // LCDの背景
    g.setColour(juce::Colour(20, 20, 20));
    g.fillRoundedRectangle(bounds, 5.0f);

    // LCD画面の枠
    g.setColour(juce::Colours::black);
    g.drawRoundedRectangle(bounds.reduced(1.0f), 5.0f, 2.0f);

    // テキスト表示エリア
    auto displayArea = bounds.reduced(5.0f);
    g.setColour(juce::Colour(0, 172, 255));
    g.setFont(juce::Font(24.0f).withStyle(juce::Font::bold));
    g.drawText(displayText, displayArea, juce::Justification::centred);

    // スキャンライン効果
    g.setColour(juce::Colours::white.withAlpha(0.1f));
    float scanLineY = (float)(juce::Time::getMillisecondCounter() % 1000) / 1000.0f * bounds.getHeight();
    g.drawHorizontalLine((int)scanLineY, bounds.getX(), bounds.getRight());
}

void SP404Display::setText(const juce::String& newText)
{
    displayText = newText;
    repaint();
}

// MainComponent implementation
MainComponent::MainComponent(AudioPluginAudioProcessor& p)
    : audioProcessor(p)
{
    setupPads();
    setupKnobs();
    setupLCD();
    setupEffectButtons();
    
    startTimer(50); // 50msごとに更新
    setSize(800, 600);
}

MainComponent::~MainComponent()
{
    stopTimer();
}

void MainComponent::paint(juce::Graphics& g)
{
    // 背景のグラデーション
    juce::ColourGradient backgroundGradient(
        juce::Colour(50, 50, 50),
        getLocalBounds().toFloat().getTopLeft(),
        juce::Colour(30, 30, 30),
        getLocalBounds().toFloat().getBottomRight(),
        true);
    g.setGradientFill(backgroundGradient);
    g.fillAll();

    // メタリックな質感を追加
    juce::ColourGradient metallic(
        juce::Colours::white.withAlpha(0.1f),
        getLocalBounds().toFloat().getTopLeft(),
        juce::Colours::transparentWhite,
        getLocalBounds().toFloat().getBottomRight(),
        true);
    g.setGradientFill(metallic);
    g.fillAll();
}

void MainComponent::resized()
{
    calculateResponsiveLayout();
}

void MainComponent::setupPads()
{
    for (int i = 0; i < 8; ++i)
    {
        auto pad = std::make_unique<SP404Pad>(juce::String(i + 1));
        pad->addListener(this);
        addAndMakeVisible(pad.get());
        pads[i] = std::move(pad);
    }
}

void MainComponent::setupKnobs()
{
    juce::StringArray knobLabels = { "TIME", "FEEDBACK", "LEVEL", "VALUE" };
    
    for (int i = 0; i < 3; ++i)
    {
        auto knob = std::make_unique<SP404Knob>(knobLabels[i]);
        knob->setRange(0.0, 100.0, 1.0);
        knob->addListener(this);
        addAndMakeVisible(knob.get());
        controlKnobs[i] = std::move(knob);
    }

    valueKnob = std::make_unique<SP404Knob>(knobLabels[3]);
    valueKnob->setRange(0.0, 100.0, 1.0);
    valueKnob->addListener(this);
    addAndMakeVisible(valueKnob.get());
}

void MainComponent::setupLCD()
{
    lcdScreen = std::make_unique<SP404Display>();
    addAndMakeVisible(lcdScreen.get());
}

void MainComponent::setupEffectButtons()
{
    juce::StringArray effectNames = { "DELAY", "FILTER", "REVERB", "ISOLATOR" };
    
    for (int i = 0; i < 4; ++i)
    {
        auto button = std::make_unique<juce::TextButton>(effectNames[i]);
        button->addListener(this);
        button->setColour(juce::TextButton::buttonColourId, juce::Colour(60, 60, 60));
        button->setColour(juce::TextButton::textColourOffId, textColour);
        addAndMakeVisible(button.get());
        effectButtons[i] = std::move(button);
    }
}

void MainComponent::calculateResponsiveLayout()
{
    auto bounds = getLocalBounds();
    
    // LCD画面のレイアウト
    auto lcdArea = bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.2));
    lcdScreen->setBounds(getScaledBounds(lcdArea.reduced(20)));

    // ノブエリアのレイアウト
    auto knobArea = bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.3));
    auto knobWidth = knobArea.getWidth() / 4;
    
    for (int i = 0; i < 3; ++i)
    {
        controlKnobs[i]->setBounds(getScaledBounds(
            knobArea.removeFromLeft(knobWidth).reduced(10)));
    }
    valueKnob->setBounds(getScaledBounds(knobArea.reduced(10)));

    // エフェクトボタンのレイアウト
    auto effectArea = bounds.removeFromTop(static_cast<int>(bounds.getHeight() * 0.2));
    auto effectWidth = effectArea.getWidth() / 4;
    
    for (int i = 0; i < 4; ++i)
    {
        effectButtons[i]->setBounds(getScaledBounds(
            effectArea.removeFromLeft(effectWidth).reduced(5)));
    }

    // パッドのレイアウト
    auto padArea = bounds;
    auto padWidth = padArea.getWidth() / 4;
    auto padHeight = padArea.getHeight() / 2;

    for (int row = 0; row < 2; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            int index = row * 4 + col;
            pads[index]->setBounds(getScaledBounds(
                juce::Rectangle<int>(col * padWidth, 
                                   row * padHeight + padArea.getY(),
                                   padWidth, 
                                   padHeight).reduced(5)));
        }
    }
}

float MainComponent::getScaleFactor() const
{
    auto bounds = getLocalBounds().toFloat();
    return juce::jmin(bounds.getWidth() / 800.0f, bounds.getHeight() / 600.0f);
}

juce::Rectangle<int> MainComponent::getScaledBounds(const juce::Rectangle<int>& original) const
{
    float scale = getScaleFactor();
    auto scaledBounds = original.toFloat();
    scaledBounds.setSize(scaledBounds.getWidth() * scale, 
                        scaledBounds.getHeight() * scale);
    return scaledBounds.toNearestInt();
}

void MainComponent::timerCallback()
{
    updateLCDDisplay();
    repaint();
}

void MainComponent::buttonClicked(juce::Button* button)
{
    // パッドのクリックを処理
    for (size_t i = 0; i < pads.size(); ++i)
    {
        if (button == pads[i].get())
        {
            handlePadTrigger(static_cast<int>(i));
            return;
        }
    }

    // エフェクトボタンのクリックを処理
    for (size_t i = 0; i < effectButtons.size(); ++i)
    {
        if (button == effectButtons[i].get())
        {
            lcdScreen->setText(button->getButtonText() + " ON");
            return;
        }
    }
}

void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    // Time Stretchパラメータの更新
    if (slider == controlKnobs[0].get())
    {
        applyTimeStretchEffect(static_cast<float>(slider->getValue() / 100.0));
    }
    
    updateLCDDisplay();
}

void MainComponent::handlePadTrigger(int padIndex)
{
    // パッドがトリガーされた時の処理
    lcdScreen->setText("PAD " + juce::String(padIndex + 1) + " TRIGGERED");
}

void MainComponent::updateLCDDisplay()
{
    // LCDディスプレイの更新処理
    // 必要に応じて現在のパラメータ値などを表示
}

void MainComponent::applyTimeStretchEffect(float amount)
{
    // Time Stretchエフェクトの適用
    // ここでaudioProcessorのパラメータを更新
    if (auto* timeParam = audioProcessor.apvts.getParameter("timeDiv"))
    {
        timeParam->setValueNotifyingHost(amount);
    }
} 