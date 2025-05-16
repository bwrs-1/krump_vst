#include "DisplayLabel.h"

DisplayLabel::DisplayLabel(const juce::String& text)
    : juce::Label("", text)
{
    setLookAndFeel(&labelLookAndFeel);
    setJustificationType(juce::Justification::centred);
    setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
}

void DisplayLabel::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(1.0f);
    
    // 背景を描画
    g.setColour(SP404LookAndFeel::background.darker(0.2f));
    g.fillRoundedRectangle(bounds, 2.0f);
    
    // 枠を描画
    g.setColour(SP404LookAndFeel::buttonColor);
    g.drawRoundedRectangle(bounds, 2.0f, 1.0f);

    // テキストを描画
    g.setColour(SP404LookAndFeel::textColor);
    g.setFont(getFont());

    if (isValueMode)
    {
        auto displayText = valueText;
        if (valueSuffix.isNotEmpty())
            displayText += " " + valueSuffix;
        g.drawText(displayText, bounds, juce::Justification::centred);
    }
    else
    {
        g.drawText(getText(), bounds, juce::Justification::centred);
    }
}

void DisplayLabel::resized()
{
    juce::Label::resized();
}

void DisplayLabel::setDisplayMode(bool isValueDisplay)
{
    isValueMode = isValueDisplay;
    repaint();
}

void DisplayLabel::setValueText(const juce::String& value, const juce::String& suffix)
{
    valueText = value;
    valueSuffix = suffix;
    if (isValueMode)
        repaint();
} 