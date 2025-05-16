#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class KrumpLookAndFeel : public juce::LookAndFeel_V4
{
public:
    KrumpLookAndFeel();

    // スライダーのカスタマイズ
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override;

    // ボタンのカスタマイズ
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override;

    // コンボボックスのカスタマイズ
    void drawComboBox(juce::Graphics& g, int width, int height,
                     bool isButtonDown, int buttonX, int buttonY,
                     int buttonW, int buttonH, juce::ComboBox& box) override;

    // フォントアクセサ
    const juce::Font& getHeaderFont() const { return headerFont; }
    const juce::Font& getMainFont() const { return mainFont; }

private:
    // カラーパレット
    juce::Colour primaryColor;      // メインカラー
    juce::Colour secondaryColor;    // アクセントカラー
    juce::Colour backgroundColor;   // 背景色
    juce::Colour textColor;         // テキストカラー

    // フォント
    juce::Font mainFont;
    juce::Font headerFont;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KrumpLookAndFeel)
}; 