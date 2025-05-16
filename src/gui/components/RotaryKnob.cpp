#include "RotaryKnob.h"

RotaryKnob::RotaryKnob(const juce::String& labelText)
    : juce::Slider(juce::Slider::RotaryHorizontalVerticalDrag,
                  juce::Slider::TextEntryBoxPosition::NoTextBox)
{
    setLookAndFeel(&knobLookAndFeel);
    setRotaryParameters(juce::MathConstants<float>::pi * 1.2f,
                       juce::MathConstants<float>::pi * 2.8f,
                       true);

    label.setText(labelText, juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    label.setFont(juce::Font("Arial", 12.0f, juce::Font::plain));
    addAndMakeVisible(label);

    setTextValueSuffix(valueSuffix);
}

void RotaryKnob::paint(juce::Graphics& g)
{
    juce::Slider::paint(g);

    // 現在の値を表示
    auto bounds = getLocalBounds();
    auto valueBounds = bounds.removeFromBottom(20);
    
    g.setColour(SP404LookAndFeel::textColor);
    g.setFont(juce::Font("Arial", 10.0f, juce::Font::plain));
    
    auto valueText = juce::String(getValue(), 1) + valueSuffix;
    g.drawText(valueText, valueBounds, juce::Justification::centred);
}

void RotaryKnob::resized()
{
    auto bounds = getLocalBounds();
    label.setBounds(bounds.removeFromTop(20));
    juce::Slider::resized();
}

void RotaryKnob::setLabelText(const juce::String& newText)
{
    label.setText(newText, juce::dontSendNotification);
}

void RotaryKnob::setValueSuffix(const juce::String& suffix)
{
    valueSuffix = suffix;
    repaint();
} 