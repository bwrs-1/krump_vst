#include "PresetButton.h"

PresetButton::PresetButton(const juce::String& name)
    : juce::TextButton(name)
{
    setLookAndFeel(&buttonLookAndFeel);
}

void PresetButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    
    // 背景色の設定
    juce::Colour baseColour = SP404LookAndFeel::highlightColor;
    
    if (isMouseOver())
        baseColour = baseColour.brighter(0.2f);
    if (isMouseButtonDown())
        baseColour = baseColour.darker(0.2f);

    // ボタンの背景を描画
    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 4.0f);

    // ボタンの枠を描画
    g.setColour(baseColour.brighter(0.1f));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);

    // テキストを描画
    g.setColour(SP404LookAndFeel::textColor);
    g.setFont(juce::Font("Arial", 14.0f, juce::Font::bold));
    
    juce::String displayText = getButtonText();
    if (presetNumber > 0)
        displayText += " " + juce::String(presetNumber);
        
    g.drawText(displayText, bounds, juce::Justification::centred);
}

void PresetButton::resized()
{
    juce::TextButton::resized();
}

void PresetButton::setPresetNumber(int number)
{
    presetNumber = number;
    repaint();
} 