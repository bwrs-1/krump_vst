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
    
    // 背景円
    g.setColour(juce::Colour(41, 43, 44));
    g.fillEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2);
    
    // 値の円弧
    auto angle = (getValue() - getMinimum()) / (getMaximum() - getMinimum()) * 2.0f * juce::MathConstants<float>::pi;
    auto lineW = radius * 0.2f;
    auto arcRadius = radius - lineW * 0.5f;
    
    juce::Path valueArc;
    valueArc.addCentredArc(center.x, center.y, arcRadius, arcRadius,
                          0.0f, -juce::MathConstants<float>::halfPi,
                          angle - juce::MathConstants<float>::halfPi,
                          true);
    
    g.setColour(juce::Colour(65, 172, 255));
    g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    
    // ラベル
    g.setColour(juce::Colour(229, 229, 229));
    g.setFont(14.0f);
    g.drawText(label, bounds, juce::Justification::centredTop);
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
    
    if (onPositionChanged)
    {
        onPositionChanged(currentX, currentY);
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
    : AudioProcessorEditor(&p), processorRef(p),
      reverbSizeKnob("Size"),
      reverbDampingKnob("Damping"),
      reverbWidthKnob("Width"),
      reverbMixKnob("Mix")
{
    // ウィンドウサイズ設定
    setResizable(true, true);
    setSize(600, 400);
    setResizeLimits(400, 300, 800, 600);

    // XYパッドの設定
    addAndMakeVisible(xyPad);
    xyPad.onPositionChanged = [this](float x, float y) {
        processorRef.apvts.getParameter("grainSpread")->setValueNotifyingHost(x);
        processorRef.apvts.getParameter("grainPitch")->setValueNotifyingHost(y);
    };

    // リバーブコントロールの設定と追加
    addAndMakeVisible(reverbSizeKnob);
    addAndMakeVisible(reverbDampingKnob);
    addAndMakeVisible(reverbWidthKnob);
    addAndMakeVisible(reverbMixKnob);

    // パラメーター接続
    reverbSizeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "reverbSize", reverbSizeKnob);
    reverbDampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "reverbDamping", reverbDampingKnob);
    reverbWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "reverbWidth", reverbWidthKnob);
    reverbMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "reverbMix", reverbMixKnob);

    // タイマー開始
    startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    stopTimer();
}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(backgroundColour);

    // XYパッドのラベル
    g.setColour(textColour);
    g.setFont(16.0f);
    g.drawText("Grain Control", 20, 10, 200, 30, juce::Justification::left);

    // リバーブセクションのラベル
    g.drawText("Reverb", getWidth() - 220, 10, 200, 30, juce::Justification::right);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    auto rightPanel = bounds.removeFromRight(200);

    // XYパッド
    xyPad.setBounds(bounds.reduced(10));

    // リバーブコントロール
    auto reverbControls = rightPanel.removeFromTop(bounds.getHeight());
    reverbControls.removeFromTop(40); // ラベル用のスペース

    auto knobSize = 80;
    auto knobSpacing = (reverbControls.getHeight() - knobSize * 4) / 3;

    reverbSizeKnob.setBounds(reverbControls.removeFromTop(knobSize));
    reverbControls.removeFromTop(knobSpacing);
    
    reverbDampingKnob.setBounds(reverbControls.removeFromTop(knobSize));
    reverbControls.removeFromTop(knobSpacing);
    
    reverbWidthKnob.setBounds(reverbControls.removeFromTop(knobSize));
    reverbControls.removeFromTop(knobSpacing);
    
    reverbMixKnob.setBounds(reverbControls.removeFromTop(knobSize));
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    // XYパッドの位置を更新
    float spread = processorRef.apvts.getParameter("grainSpread")->getValue();
    float pitch = processorRef.apvts.getParameter("grainPitch")->getValue();
    xyPad.setPosition(spread, pitch);

    repaint();
}