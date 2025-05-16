#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SP404LookAndFeel : public juce::LookAndFeel_V4
{
public:
    SP404LookAndFeel();
    ~SP404LookAndFeel() override = default;

    // カラー定義
    static const juce::Colour background;
    static const juce::Colour buttonColor;
    static const juce::Colour accentColor;
    static const juce::Colour highlightColor;
    static const juce::Colour textColor;

    // フォント
    static constexpr const char* customFontPath = "/Users/BWRS1_dev/krump_vst-1/Bon_en_ji-Regular.otf";
    juce::Font getCustomFont(float size, juce::Font::FontStyleFlags style = juce::Font::plain) const;

    // ボタンの描画
    void drawButtonBackground(juce::Graphics& g, juce::Button& button,
                            const juce::Colour& backgroundColour,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override;

    void drawButtonText(juce::Graphics& g, juce::TextButton& button,
                       bool shouldDrawButtonAsHighlighted,
                       bool shouldDrawButtonAsDown) override;

    // スライダーの描画
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPosProportional, float rotaryStartAngle,
                         float rotaryEndAngle, juce::Slider& slider) override;

    // ラベルの描画
    void drawLabel(juce::Graphics& g, juce::Label& label) override;

    // コンボボックスのスタイル
    void drawComboBox(juce::Graphics& g, int width, int height,
                     bool isButtonDown, int buttonX, int buttonY,
                     int buttonW, int buttonH, juce::ComboBox& box) override;

private:
    juce::Font mainFont;
    juce::Font headerFont;
    juce::ReferenceCountedObjectPtr<juce::Typeface> customTypeface;
    void loadCustomFont();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SP404LookAndFeel)
}; 