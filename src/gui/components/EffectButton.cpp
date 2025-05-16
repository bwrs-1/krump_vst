#include "EffectButton.h"

EffectButton::EffectButton(const juce::String& name)
    : juce::TextButton(name)
{
    setLookAndFeel(&buttonLookAndFeel);
    setClickingTogglesState(true);
}

void EffectButton::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    
    // 背景色の設定
    juce::Colour baseColour = active ? SP404LookAndFeel::accentColor 
                                   : SP404LookAndFeel::buttonColor;
    
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
    g.drawText(getButtonText(), bounds, juce::Justification::centred);
}

void EffectButton::resized()
{
    juce::TextButton::resized();
}

void EffectButton::setActive(bool shouldBeActive)
{
    active = shouldBeActive;
    repaint();
} 