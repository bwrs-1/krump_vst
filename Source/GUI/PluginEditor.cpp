#include "PluginEditor.h"
#include "../Core/PluginProcessor.h"

// ===== CustomLookAndFeel 実装 =====
CustomLookAndFeel::CustomLookAndFeel() {
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffe53935)); // レッド
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xff222222)); // ダーク
    setColour(juce::Slider::thumbColourId, juce::Colour(0xffe53935));
    setColour(juce::Label::textColourId, juce::Colours::white);
    setColour(juce::Label::backgroundColourId, juce::Colour(0xff222222));
    setColour(juce::Label::outlineColourId, juce::Colour(0xffe53935));
}

void CustomLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional,
                                         float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) {
    auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(8);
    float radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    float centreX = bounds.getCentreX();
    float centreY = bounds.getCentreY();
    float rx = centreX - radius;
    float ry = centreY - radius;
    float rw = radius * 2.0f;
    float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    // 背景リング
    g.setColour(juce::Colour(0xff333333));
    g.fillEllipse(rx, ry, rw, rw);
    // シャドウ
    g.setColour(juce::Colours::black.withAlpha(0.3f));
    g.fillEllipse(rx+2, ry+2, rw, rw);
    // アウトライン
    g.setColour(findColour(juce::Slider::rotarySliderOutlineColourId));
    g.drawEllipse(rx, ry, rw, rw, 2.0f);
    // フィル
    g.setColour(findColour(juce::Slider::rotarySliderFillColourId));
    juce::Path valueArc;
    valueArc.addCentredArc(centreX, centreY, radius-4, radius-4, 0.0f, rotaryStartAngle, angle, true);
    g.strokePath(valueArc, juce::PathStrokeType(4.0f));
    // ノブ
    float knobRadius = radius * 0.6f;
    g.setColour(juce::Colour(0xffe53935));
    g.fillEllipse(centreX - knobRadius, centreY - knobRadius, knobRadius * 2, knobRadius * 2);
    // インジケータ
    g.setColour(juce::Colours::white);
    juce::Path p;
    float pointerLen = radius * 0.8f;
    float pointerThickness = 3.0f;
    p.addRectangle(-pointerThickness * 0.5f, -pointerLen, pointerThickness, pointerLen);
    p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    g.fillPath(p);
}

void CustomLookAndFeel::drawLabel(juce::Graphics& g, juce::Label& label) {
    auto bounds = label.getLocalBounds().toFloat();
    g.setColour(label.findColour(juce::Label::backgroundColourId));
    g.fillRoundedRectangle(bounds, 6.0f);
    g.setColour(label.findColour(juce::Label::outlineColourId));
    g.drawRoundedRectangle(bounds, 6.0f, 2.0f);
    g.setColour(label.findColour(juce::Label::textColourId));
    g.setFont(label.getFont().boldened().withHeight(16.0f));
    g.drawFittedText(label.getText(), label.getLocalBounds(), juce::Justification::centred, 1);
}

// ===== PluginEditor =====
KrumpVSTAudioProcessorEditor::KrumpVSTAudioProcessorEditor(KrumpVSTAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setLookAndFeel(&customLnf);
    setSize(800, 600);
    setResizable(true, true);
    setResizeLimits(400, 300, 1600, 1200);

    // フォントファイルの読み込み
    juce::File fontFile("/Users/bwrs1/Documents/krump_vst/Bon_en_ji-Regular .otf");
    if (fontFile.existsAsFile()) {
        auto fontStream = fontFile.createInputStream();
        if (fontStream != nullptr) {
            juce::MemoryBlock fontData;
            fontStream->readIntoMemoryBlock(fontData);
            juce::Typeface::Ptr typeface = juce::Typeface::createSystemTypefaceFor(fontData.getData(), fontData.getSize());
            if (typeface != nullptr) {
                customFont = juce::Font(typeface);
                customFont.setHeight(40.0f);
            }
        }
    }
    if (customFont.getTypefacePtr() == nullptr) {
        // フォント読み込み失敗時はデフォルト
        customFont = juce::Font(40.0f);
    }

    // スライダー・ラベル初期化
    auto& apvts = audioProcessor.apvts;
    auto makeSlider = [](juce::Slider& s) {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    };
    makeSlider(roomSizeSlider);   roomSizeLabel.setText("Room Size", juce::dontSendNotification);
    makeSlider(dampingSlider);    dampingLabel.setText("Damping", juce::dontSendNotification);
    makeSlider(wetSlider);        wetLabel.setText("Wet", juce::dontSendNotification);
    makeSlider(drySlider);        dryLabel.setText("Dry", juce::dontSendNotification);
    makeSlider(widthSlider);      widthLabel.setText("Width", juce::dontSendNotification);
    makeSlider(freezeSlider);     freezeLabel.setText("Freeze", juce::dontSendNotification);

    addAndMakeVisible(roomSizeSlider);   addAndMakeVisible(roomSizeLabel);
    addAndMakeVisible(dampingSlider);    addAndMakeVisible(dampingLabel);
    addAndMakeVisible(wetSlider);        addAndMakeVisible(wetLabel);
    addAndMakeVisible(drySlider);        addAndMakeVisible(dryLabel);
    addAndMakeVisible(widthSlider);      addAndMakeVisible(widthLabel);
    addAndMakeVisible(freezeSlider);     addAndMakeVisible(freezeLabel);

    roomSizeAttach  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "RoomSize",  roomSizeSlider);
    dampingAttach   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "Damping",   dampingSlider);
    wetAttach       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "Wet",       wetSlider);
    dryAttach       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "Dry",       drySlider);
    widthAttach     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "Width",     widthSlider);
    freezeAttach    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "Freeze",    freezeSlider);
}

KrumpVSTAudioProcessorEditor::~KrumpVSTAudioProcessorEditor()
{
}

void KrumpVSTAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff181818)); // ダークグレー背景
    g.setColour(juce::Colours::white);
    g.setFont(customFont);
    g.drawFittedText("BUCK REVERB", getLocalBounds().removeFromTop(60), juce::Justification::centred, 1);
}

void KrumpVSTAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(40).removeFromTop(getHeight() - 80);
    auto sliderW = area.getWidth() / 6;
    auto sliderH = area.getHeight() - 30;
    roomSizeSlider.setBounds(area.removeFromLeft(sliderW).reduced(10, 20));
    roomSizeLabel.setBounds(roomSizeSlider.getX(), roomSizeSlider.getBottom(), sliderW, 20);
    dampingSlider.setBounds(area.removeFromLeft(sliderW).reduced(10, 20));
    dampingLabel.setBounds(dampingSlider.getX(), dampingSlider.getBottom(), sliderW, 20);
    wetSlider.setBounds(area.removeFromLeft(sliderW).reduced(10, 20));
    wetLabel.setBounds(wetSlider.getX(), wetSlider.getBottom(), sliderW, 20);
    drySlider.setBounds(area.removeFromLeft(sliderW).reduced(10, 20));
    dryLabel.setBounds(drySlider.getX(), drySlider.getBottom(), sliderW, 20);
    widthSlider.setBounds(area.removeFromLeft(sliderW).reduced(10, 20));
    widthLabel.setBounds(widthSlider.getX(), widthSlider.getBottom(), sliderW, 20);
    freezeSlider.setBounds(area.removeFromLeft(sliderW).reduced(10, 20));
    freezeLabel.setBounds(freezeSlider.getX(), freezeSlider.getBottom(), sliderW, 20);
}