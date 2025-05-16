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
    
    g.setColour(juce::Colour(30, 30, 30).withAlpha(0.6f));
    g.fillEllipse(center.x - radius + 2, center.y - radius + 2, radius * 2, radius * 2);
    
    juce::ColourGradient gradient(
        juce::Colour(50, 50, 50),
        center,
        juce::Colour(35, 35, 35),
        {center.x, center.y + radius},
        true);
    g.setGradientFill(gradient);
    g.fillEllipse(center.x - radius, center.y - radius, radius * 2, radius * 2);
    
    auto angle = (getValue() - getMinimum()) / (getMaximum() - getMinimum()) * 2.0f * juce::MathConstants<float>::pi;
    auto lineW = radius * 0.15f;
    auto arcRadius = radius - lineW * 0.5f;
    
    juce::Path backgroundArc;
    backgroundArc.addCentredArc(center.x, center.y, arcRadius, arcRadius,
                               0.0f, -juce::MathConstants<float>::halfPi,
                               juce::MathConstants<float>::pi * 1.5f,
                               true);
    g.setColour(juce::Colour(60, 60, 60));
    g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

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
    
    g.setColour(juce::Colour(229, 229, 229));
    g.setFont(16.0f);
    g.drawText(label, bounds, juce::Justification::centredTop);
    
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

    g.setColour(isButtonDown ? juce::Colour(65, 172, 255) : juce::Colour(41, 43, 44));
    g.fillRoundedRectangle(bounds, 4.0f);

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

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), 
      processorRef(p),
      timeDiv1_1Button("1/1"),
      timeDiv1_2Button("1/2"),
      timeDiv1_4Button("1/4"),
      timeDiv1_8Button("1/8"),
      timeDiv1_16Button("1/16"),
      timeDiv1_32Button("1/32"),
      timeDiv1_64Button("1/64"),
      mixKnob("Mix"),
      pitchKnob("Pitch")
{
    setResizable(true, true);
    setSize(800, 400);
    setResizeLimits(600, 300, 1200, 600);

    addAndMakeVisible(timeDiv1_1Button);
    addAndMakeVisible(timeDiv1_2Button);
    addAndMakeVisible(timeDiv1_4Button);
    addAndMakeVisible(timeDiv1_8Button);
    addAndMakeVisible(timeDiv1_16Button);
    addAndMakeVisible(timeDiv1_32Button);
    addAndMakeVisible(timeDiv1_64Button);
    addAndMakeVisible(mixKnob);
    addAndMakeVisible(pitchKnob);

    mixKnob.setRange(0.0, 100.0, 0.1);
    pitchKnob.setRange(-12.0, 12.0, 0.1);

    timeDiv1_1Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_1Button);
    timeDiv1_2Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_2Button);
    timeDiv1_4Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_4Button);
    timeDiv1_8Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_8Button);
    timeDiv1_16Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_16Button);
    timeDiv1_32Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_32Button);
    timeDiv1_64Attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.apvts, "timeDiv", timeDiv1_64Button);

    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "mix", mixKnob);
    pitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.apvts, "pitch", pitchKnob);

    startTimerHz(30);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    stopTimer();
}

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient backgroundGradient(
        juce::Colour(40, 40, 40),
        {0.0f, 0.0f},
        juce::Colour(30, 30, 30),
        {static_cast<float>(getWidth()), static_cast<float>(getHeight())},
        false);
    g.setGradientFill(backgroundGradient);
    g.fillAll();
    
    g.setColour(textColour);
    g.setFont(24.0f);
    g.drawText("KRUMP VST", 20, 10, 200, 30, juce::Justification::left);
    
    g.setFont(12.0f);
    g.setColour(textColour.withAlpha(0.7f));
    g.drawText("v1.0.0", getWidth() - 60, getHeight() - 20, 50, 20, juce::Justification::right);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    const float aspectRatio = 2.0f;
    
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
    
    auto mainArea = bounds;
    mainArea.removeFromTop(40);
    
    auto buttonArea = mainArea.removeFromTop(40);
    auto buttonWidth = buttonArea.getWidth() / 7;
    
    timeDiv1_1Button.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    timeDiv1_2Button.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    timeDiv1_4Button.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    timeDiv1_8Button.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    timeDiv1_16Button.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    timeDiv1_32Button.setBounds(buttonArea.removeFromLeft(buttonWidth).reduced(5));
    timeDiv1_64Button.setBounds(buttonArea.reduced(5));
    
    auto controlArea = mainArea.removeFromBottom(120);
    auto knobWidth = controlArea.getWidth() / 2;
    
    mixKnob.setBounds(controlArea.removeFromLeft(knobWidth).reduced(10));
    pitchKnob.setBounds(controlArea.reduced(10));
}

void AudioPluginAudioProcessorEditor::timerCallback()
{
    repaint();
}