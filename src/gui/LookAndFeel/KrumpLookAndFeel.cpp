#include "KrumpLookAndFeel.h"

KrumpLookAndFeel::KrumpLookAndFeel()
{
    // カラーパレットの設定
    primaryColor = juce::Colour(0xFF2C3E50);      // ダークブルー
    secondaryColor = juce::Colour(0xFF3498DB);    // ブライトブルー
    backgroundColor = juce::Colour(0xFF1A1A1A);   // ダークグレー
    textColor = juce::Colour(0xFFECF0F1);        // オフホワイト

    // フォントの設定
    mainFont = juce::Font("Helvetica Neue", 14.0f, juce::Font::plain);
    headerFont = juce::Font("Helvetica Neue", 18.0f, juce::Font::bold);

    setColour(juce::Slider::thumbColourId, secondaryColor);
    setColour(juce::Slider::trackColourId, primaryColor);
    setColour(juce::Slider::backgroundColourId, backgroundColor);
    setColour(juce::Label::textColourId, textColor);
}

void KrumpLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                      float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                      juce::Slider& slider)
{
    // スライダーの基本的な設定
    const float radius = juce::jmin(width / 2, height / 2) - 4.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    // 背景円を描画
    g.setColour(backgroundColor);
    g.fillEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2);

    // 外枠を描画
    g.setColour(primaryColor);
    g.drawEllipse(centreX - radius, centreY - radius, radius * 2, radius * 2, 2.0f);

    // インジケーターラインを描画
    const float lineThickness = 3.0f;
    g.setColour(secondaryColor);

    juce::Path p;
    p.addRectangle(-lineThickness * 0.5f, -radius, lineThickness, radius * 0.33f);

    g.fillPath(p, juce::AffineTransform::rotation(angle).translated(centreX, centreY));

    // 値を表示
    if (slider.isEnabled())
    {
        g.setColour(textColor);
        g.setFont(mainFont);
        const juce::String text = slider.getTextFromValue(slider.getValue());
        g.drawText(text, x, y + height - 15, width, 15, juce::Justification::centred, false);
    }
}

void KrumpLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                          const juce::Colour& backgroundColour,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown)
{
    const float cornerSize = 6.0f;
    const juce::Rectangle<float> bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

    // ボタンの状態に応じて色を選択
    juce::Colour baseColour = shouldDrawButtonAsDown ? secondaryColor
                                                    : shouldDrawButtonAsHighlighted ? primaryColor.brighter(0.2f)
                                                                                  : primaryColor;

    g.setColour(baseColour);
    g.fillRoundedRectangle(bounds, cornerSize);

    // ボーダーを描画
    g.setColour(baseColour.brighter());
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
}

void KrumpLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height,
                                   bool isButtonDown, int buttonX, int buttonY,
                                   int buttonW, int buttonH, juce::ComboBox& box)
{
    const float cornerSize = 6.0f;
    const juce::Rectangle<float> bounds(0, 0, width, height);

    g.setColour(primaryColor);
    g.fillRoundedRectangle(bounds, cornerSize);

    // ドロップダウン矢印を描画
    const float arrowX = width - 20.0f;
    const float arrowY = height * 0.5f;
    const float arrowSize = 6.0f;

    juce::Path arrow;
    arrow.startNewSubPath(arrowX - arrowSize, arrowY - arrowSize);
    arrow.lineTo(arrowX, arrowY + arrowSize);
    arrow.lineTo(arrowX + arrowSize, arrowY - arrowSize);
    arrow.closeSubPath();

    g.setColour(textColor);
    g.fillPath(arrow);
} 