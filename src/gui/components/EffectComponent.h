#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../../audio/effects/Effect.h"
#include "../LookAndFeel/KrumpLookAndFeel.h"

class EffectComponent : public juce::Component,
                       public juce::DragAndDropTarget
{
public:
    explicit EffectComponent(Effect* effect);
    ~EffectComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    // ドラッグ＆ドロップ関連
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent&) override;

    bool isInterestedInDragSource(const SourceDetails&) override;
    void itemDragEnter(const SourceDetails&) override;
    void itemDragMove(const SourceDetails&) override;
    void itemDragExit(const SourceDetails&) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;

    // パラメーター更新
    void updateParameters();
    void parameterChanged(int parameterIndex, float newValue);
    int getIndexInParent() const;

    juce::TextButton removeButton;
    juce::TextButton bypassButton;

private:
    Effect* effect;
    KrumpLookAndFeel lookAndFeel;
    juce::Label titleLabel;
    juce::OwnedArray<juce::Slider> sliders;
    juce::OwnedArray<juce::Label> labels;
    juce::ComponentDragger dragger;
    bool isBeingDragged = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EffectComponent)
}; 