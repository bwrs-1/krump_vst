#include "SP404LookAndFeel.h"
#include <juce_core/juce_core.h>

// カラー定義
const juce::Colour SP404LookAndFeel::background = juce::Colour(0xFF1E1E1E);
const juce::Colour SP404LookAndFeel::buttonColor = juce::Colour(0xFF2D2D2D);
const juce::Colour SP404LookAndFeel::accentColor = juce::Colour(0xFF00BFFF);
const juce::Colour SP404LookAndFeel::highlightColor = juce::Colour(0xFF4CAF50);
const juce::Colour SP404LookAndFeel::textColor = juce::Colour(0xFFE0E0E0);

SP404LookAndFeel::SP404LookAndFeel()
{
    loadCustomFont();
    setColour(juce::ResizableWindow::backgroundColourId, background);
    setColour(juce::TextButton::buttonColourId, buttonColor);
    setColour(juce::TextButton::textColourOffId, textColor);
    setColour(juce::Slider::rotarySliderFillColourId, accentColor);
    setColour(juce::Slider::rotarySliderOutlineColourId, buttonColor);
    setColour(juce::Slider::thumbColourId, highlightColor);
    setColour(juce::Label::textColourId, textColor);
}

void SP404LookAndFeel::loadCustomFont()
{
    juce::File fontFile(customFontPath);
    if (fontFile.existsAsFile()) {
        juce::MemoryBlock fontData;
        if (fontFile.loadFileAsData(fontData)) {
            customTypeface = juce::Typeface::createSystemTypefaceFor(fontData.getData(), fontData.getSize());
        }
    }
}

juce::Font SP404LookAndFeel::getCustomFont(float size, juce::Font::FontStyleFlags style) const
{
    if (customTypeface)
        return juce::Font(customTypeface.get()).withHeight(size).withStyle(style);
    return juce::Font("Arial", size, style);
}

void SP404LookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                          const juce::Colour& backgroundColour,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(2.0f);
    auto baseColour = backgroundColour;

    if (shouldDrawButtonAsDown)
        baseColour = baseColour.darker(0.2f);
    else if (shouldDrawButtonAsHighlighted)
        baseColour = baseColour.brighter(0.2f);

    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, 4.0f);

    g.setColour(baseColour.brighter(0.1f));
    g.drawRoundedRectangle(bounds, 4.0f, 1.0f);
}

void SP404LookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                    bool shouldDrawButtonAsHighlighted,
                                    bool shouldDrawButtonAsDown)
{
    g.setColour(button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                         : juce::TextButton::textColourOffId)
                .withMultipliedAlpha(button.isEnabled() ? 1.0f : 0.5f));
    g.setFont(getCustomFont(14.0f, juce::Font::bold));
    g.drawText(button.getButtonText(), button.getLocalBounds(),
               juce::Justification::centred, true);
}

void SP404LookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                      float sliderPosProportional, float rotaryStartAngle,
                                      float rotaryEndAngle, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // 背景
    g.setColour(slider.findColour(juce::Slider::rotarySliderOutlineColourId));
    g.fillEllipse(rx, ry, rw, rw);

    // 値の表示
    g.setColour(slider.findColour(juce::Slider::rotarySliderFillColourId));
    juce::Path p;
    p.addPieSegment(rx, ry, rw, rw, rotaryStartAngle, angle, 0.0f);
    g.fillPath(p);

    // ノブ
    g.setColour(slider.findColour(juce::Slider::thumbColourId));
    auto knobRadius = radius * 0.2f;
    auto knobX = centreX + radius * 0.8f * std::cos(angle - juce::MathConstants<float>::halfPi);
    auto knobY = centreY + radius * 0.8f * std::sin(angle - juce::MathConstants<float>::halfPi);
    g.fillEllipse(knobX - knobRadius, knobY - knobRadius, knobRadius * 2.0f, knobRadius * 2.0f);
}

void SP404LookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label)
{
    g.fillAll(label.findColour(juce::Label::backgroundColourId));
    if (!label.isBeingEdited())
    {
        auto alpha = label.isEnabled() ? 1.0f : 0.5f;
        const juce::Font font(getCustomFont(14.0f, juce::Font::bold));
        g.setColour(label.findColour(juce::Label::textColourId).withMultipliedAlpha(alpha));
        g.setFont(font);
        auto textArea = label.getBorderSize().subtractedFrom(label.getLocalBounds());
        g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                        juce::jmax(1, (int)(textArea.getHeight() / font.getHeight())),
                        label.getMinimumHorizontalScale());
    }
} 