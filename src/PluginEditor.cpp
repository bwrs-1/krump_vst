#include "PluginProcessor.h"
#include "PluginEditor.h"

void ModernPadButton::paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = getLocalBounds().toFloat().reduced(2.0f);
    float cornerSize = 6.0f;
    
    // Background
    g.setColour(shouldDrawButtonAsDown ? juce::Colour(255, 165, 0) : 
                shouldDrawButtonAsHighlighted ? juce::Colour(80, 80, 80) : juce::Colour(40, 40, 40));
    g.fillRoundedRectangle(bounds, cornerSize);
    
    // Border
    g.setColour(juce::Colour(100, 100, 100));
    g.drawRoundedRectangle(bounds, cornerSize, 1.0f);
}

ModernKnob::ModernKnob()
{
    setSliderStyle(juce::Slider::RotaryVerticalDrag);
    setRotaryParameters(juce::MathConstants<float>::pi * 1.2f,
                       juce::MathConstants<float>::pi * 2.8f,
                       true);
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(255, 165, 0));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(40, 40, 40));
    setColour(juce::Slider::thumbColourId, juce::Colour(200, 200, 200));
    setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
}

AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p)
{
    setSize(800, 500);
    
    // Initialize pads
    for (int i = 0; i < numPads; ++i)
    {
        pads[i] = std::make_unique<ModernPadButton>();
        addAndMakeVisible(*pads[i]);
    }
    
    // Initialize knobs
    for (int i = 0; i < numKnobs; ++i)
    {
        knobs[i] = std::make_unique<ModernKnob>();
        addAndMakeVisible(*knobs[i]);
    }
    
    // Setup display
    addAndMakeVisible(displayLabel);
    displayLabel.setColour(juce::Label::backgroundColourId, juce::Colour(20, 20, 20));
    displayLabel.setColour(juce::Label::textColourId, juce::Colour(255, 165, 0));
    displayLabel.setJustificationType(juce::Justification::centred);
    displayLabel.setFont(juce::Font(24.0f));
    displayLabel.setText("SP-404 MK2", juce::dontSendNotification);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor() = default;

void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(30, 30, 30));
    
    // Draw panel details
    g.setColour(juce::Colour(50, 50, 50));
    g.drawRect(getLocalBounds(), 2);
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(20);
    
    // Display area
    auto displayArea = area.removeFromTop(60);
    displayLabel.setBounds(displayArea.reduced(10));
    
    area.removeFromTop(20); // Spacing
    
    // Pads area
    auto padsArea = area.removeFromBottom(300);
    int padSize = 65;
    int padGap = 10;
    int startX = (getWidth() - (4 * (padSize + padGap))) / 2;
    int startY = padsArea.getY();
    
    for (int i = 0; i < numPads; ++i)
    {
        int row = i / 4;
        int col = i % 4;
        pads[i]->setBounds(startX + col * (padSize + padGap),
                          startY + row * (padSize + padGap),
                          padSize, padSize);
    }
    
    // Knobs area
    int knobSize = 70;
    int knobGap = 20;
    startX = (getWidth() - (numKnobs * (knobSize + knobGap))) / 2;
    
    for (int i = 0; i < numKnobs; ++i)
    {
        knobs[i]->setBounds(startX + i * (knobSize + knobGap),
                           area.getY(),
                           knobSize, knobSize);
    }
}