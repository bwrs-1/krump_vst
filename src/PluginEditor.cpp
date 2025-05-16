#include "PluginProcessor.h"
#include "PluginEditor.h"

ModernDial::ModernDial(const juce::String& labelText)
    : juce::Slider(juce::Slider::RotaryHorizontalVerticalDrag,
                  juce::Slider::TextBoxBelow),
      label(labelText)
{
    setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 15);
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(229, 229, 229));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
}

void ModernDial::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto center = bounds.getCentre();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.35f;
    
    // 背景の影
    g.setColour(juce::Colour(30, 30, 30).withAlpha(0.6f));
    g.fillEllipse(center.x - radius + 2, center.y - radius + 2, radius * 2, radius * 2);
    
    // 背景円
    juce::ColourGradient gradient(
        juce::Colour(50, 50, 50),
        center,
        juce::Colour(35, 35, 35),
        {center.x, center.y + radius},
        true);
    g.setGradientFill(gradient);
    g.fillEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2);
    
    // 値の円弧
    auto angle = (getValue() - getMinimum()) / (getMaximum() - getMinimum()) * 2.0f * juce::MathConstants<float>::pi;
    auto lineW = radius * 0.15f;
    auto arcRadius = radius - lineW * 0.5f;
    
    // 背景の円弧
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(center.x, center.y, arcRadius, arcRadius,
                               0.0f, -juce::MathConstants<float>::halfPi,
                               juce::MathConstants<float>::pi * 1.5f,
                               true);
    g.setColour(juce::Colour(60, 60, 60));
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    // 値の円弧
    juce::Path valueArc;
    valueArc.addCentredArc(center.x, center.y, arcRadius, arcRadius,
                          0.0f, -juce::MathConstants<float>::halfPi,
                          angle - juce::MathConstants<float>::halfPi,
                          true);
    
    juce::ColourGradient arcGradient(
        juce::Colour(65, 172, 255),
        {center.x - radius, center.y},
        juce::Colour(65, 172, 255).brighter(0.2f),
        {center.x + radius, center.y},
        true);
    g.setGradientFill(arcGradient);
    g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // ラベル
    g.setColour(juce::Colour(229, 229, 229));
    g.setFont(16.0f);
    g.drawText(label, bounds, juce::Justification::centredTop);
    
    // 値
    g.setFont(14.0f);
    g.drawText(juce::String(static_cast<int>(getValue())), bounds, juce::Justification::centred);
}

TimeButton::TimeButton(const juce::String& buttonText)
    : juce::TextButton(buttonText)
{
    setClickingTogglesState(true);
}

void TimeButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    auto isButtonDown = isDown() || getToggleState();

    // 背景
    g.setColour(isButtonDown ? juce::Colour(65, 172, 255) : juce::Colour(41, 43, 44));
    g.fillRoundedRectangle(bounds, 4.0f);

    // テキスト
    g.setColour(isButtonDown ? juce::Colours::black : juce::Colour(229, 229, 229));
    g.setFont(16.0f);
    g.drawText(getButtonText(), bounds, juce::Justification::centred);
}

void TimeButton::resized()
{
    auto bounds = getLocalBounds().toFloat();
    powerSymbol.clear();
    powerSymbol.addEllipse(bounds.reduced(bounds.getWidth() * 0.3f));
}

XYPad::XYPad()
{
    setOpaque(false);
}

void XYPad::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    
    // 背景
    g.setColour(juce::Colour(41, 43, 44));
    g.fillRoundedRectangle(bounds, 10.0f);
    
    // グリッド線
    g.setColour(juce::Colour(60, 63, 65));
    for (int i = 1; i < 4; ++i)
    {
        float x = bounds.getX() + (bounds.getWidth() * i / 4);
        float y = bounds.getY() + (bounds.getHeight() * i / 4);
        g.drawLine(x, bounds.getY(), x, bounds.getBottom(), 1.0f);
        g.drawLine(bounds.getX(), y, bounds.getRight(), y, 1.0f);
    }
    
    // カーソル
    float x = bounds.getX() + (currentX * bounds.getWidth());
    float y = bounds.getY() + ((1.0f - currentY) * bounds.getHeight());
    float cursorSize = 10.0f;
    
    g.setColour(juce::Colour(65, 172, 255));
    g.fillEllipse(x - cursorSize/2, y - cursorSize/2, cursorSize, cursorSize);

    // 軸ラベル
    g.setColour(juce::Colour(229, 229, 229));
    g.setFont(12.0f);
    g.drawText("Spread", bounds.getX(), bounds.getBottom() - 20, bounds.getWidth(), 20, juce::Justification::centred);
    g.addTransform(juce::AffineTransform::rotation(-juce::MathConstants<float>::halfPi, bounds.getX() + 20, bounds.getCentreY()));
    g.drawText("Pitch", bounds.getX(), bounds.getCentreY() - 40, 80, 20, juce::Justification::centred);
}

void XYPad::mouseDown(const juce::MouseEvent& e)
{
    isDragging = true;
    updatePosition(e.x, e.y);
}

void XYPad::mouseDrag(const juce::MouseEvent& e)
{
    if (isDragging)
    {
        updatePosition(e.x, e.y);
    }
}

void XYPad::mouseUp(const juce::MouseEvent&)
{
    isDragging = false;
}

void XYPad::updatePosition(float x, float y)
{
    auto bounds = getLocalBounds().toFloat();
    currentX = juce::jlimit(0.0f, 1.0f, x / bounds.getWidth());
    currentY = juce::jlimit(0.0f, 1.0f, 1.0f - (y / bounds.getHeight()));
    
    if (onPositionChanged && isEnabled())
    {
        try
        {
            onPositionChanged(currentX, currentY);
        }
        catch (...)
        {
            // コールバックでエラーが発生した場合の処理
            jassertfalse; // デバッグビルドでアサート
        }
    }
    
    repaint();
}

void XYPad::setPosition(float newX, float newY)
{
    currentX = juce::jlimit(0.0f, 1.0f, newX);
    currentY = juce::jlimit(0.0f, 1.0f, newY);
    repaint();
}

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), 
      processorRef(p),
      timeDiv1_2Button("1/2"),
      timeDiv1_4Button("1/4"),
      timeDiv1_8Button("1/8"),
      timeDiv1_16Button("1/16"),
      mixKnob("Mix")
{
    // ウィンドウサイズ設定
    setResizable(true, true);
    setSize(600, 400);
    setResizeLimits(400, 300, 800, 600);

    // XYパッドの設定
    addAndMakeVisible(xyPad);
    xyPad.onPositionChanged = [this](float x, float y) {
        if (auto* processor = dynamic_cast<AudioPluginAudioProcessor*>(getAudioProcessor()))
        {
            if (auto* spreadParam = processor->apvts.getParameter("grainSpread"))
                spreadParam->setValueNotifyingHost(x);
            if (auto* pitchParam = processor->apvts.getParameter("grainPitch"))
                pitchParam->setValueNotifyingHost(y);
        }
    };

    // タイム分割ボタンの追加
    addAndMakeVisible(timeDiv1_2Button);
    addAndMakeVisible(timeDiv1_4Button);
    addAndMakeVisible(timeDiv1_8Button);
    addAndMakeVisible(timeDiv1_16Button);

    // その他のコントロールの追加
    addAndMakeVisible(mixKnob);

    // パラメーター接続
    timeDiv1_2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_2Button);
    timeDiv1_4Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_4Button);
    timeDiv1_8Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_8Button);
    timeDiv1_16Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_16Button);

    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "mix", mixKnob);

    // タイマー開始
    startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    stopTimer();
}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    // 背景のグラデーション
    juce::ColourGradient backgroundGradient(
        juce::Colour(40, 40, 40),
        {0.0f, 0.0f},
        juce::Colour(30, 30, 30),
        {static_cast<float>(getWidth()), static_cast<float>(getHeight())},
        false);
    g.setGradientFill(backgroundGradient);
    g.fillAll();
    
    // タイトルとラベル
    g.setColour(textColour);
    g.setFont(20.0f);
    g.drawText("KRUMP VST", 20, 10, 200, 30, juce::Justification::left);
    
    // バージョン情報
    g.setFont(12.0f);
    g.setColour(textColour.withAlpha(0.7f));
    g.drawText("v1.0.0", getWidth() - 60, getHeight() - 20, 50, 20, juce::Justification::right);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    const float aspectRatio = 4.0f / 3.0f;
    
    // ウィンドウのアスペクト比を維持
    float width = bounds.getWidth();
    float height = bounds.getHeight();
    float targetHeight = width / aspectRatio;
    
    if (targetHeight > height)
    {
        width = height * aspectRatio;
        bounds = bounds.withSizeKeepingCentre(static_cast<int>(width), height);
    }
    else
    {
        bounds = bounds.withSizeKeepingCentre(width, static_cast<int>(targetHeight));
    }
    
    bounds = bounds.reduced(20);
    
    // メインエリアの設定
    auto mainArea = bounds;
    
    // XYパッド
    auto xyPadArea = mainArea.reduced(10);
    xyPadArea.removeFromTop(40); // タイトル用のスペース
    xyPad.setBounds(xyPadArea);
    
    // タイム分割ボタン
    auto buttonArea = mainArea.removeFromTop(40);
    auto buttonWidth = buttonArea.getWidth() / 4;
    
    timeDiv1_2Button.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    timeDiv1_4Button.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    timeDiv1_8Button.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    timeDiv1_16Button.setBounds(buttonArea.reduced(5));
    
    // ミックスコントロール
    auto controlArea = mainArea.removeFromBottom(mainArea.getHeight() * 0.25f);
    mixKnob.setBounds(controlArea.reduced(10));
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    // XYパッドの位置を更新
    float spread = processorRef.apvts.getParameter("grainSpread")->getValue();
    float pitch = processorRef.apvts.getParameter("grainPitch")->getValue();
    xyPad.setPosition(spread, pitch);

    repaint();
}