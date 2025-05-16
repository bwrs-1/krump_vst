#include "EffectComponent.h"
#include "../../audio/effects/Effect.h"

// ドラッグ＆ドロップ用のインターフェース
class DragAndDropInterface
{
public:
    virtual ~DragAndDropInterface() = default;
    virtual void handleEffectDragAndDrop(int fromIndex, int toIndex) = 0;
};

EffectComponent::EffectComponent(Effect* effect) : effect(effect)
{
    // タイトルラベルの設定
    titleLabel.setText(effect->getName(), juce::dontSendNotification);
    titleLabel.setFont(juce::Font(18.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // バイパスボタンの設定
    bypassButton.setButtonText("Bypass");
    bypassButton.setToggleState(!effect->isEnabled, juce::dontSendNotification);
    bypassButton.onClick = [this, effect]
    {
        effect->isEnabled = !bypassButton.getToggleState();
    };
    addAndMakeVisible(bypassButton);

    // 削除ボタンの設定
    removeButton.setButtonText("X");
    removeButton.setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    addAndMakeVisible(removeButton);

    // パラメーターコントロールの作成
    const auto paramNames = effect->getParameterNames();
    const auto paramLabels = effect->getParameterLabels();
    const auto paramRanges = effect->getParameterRanges();

    for (int i = 0; i < effect->getNumParameters(); ++i)
    {
        auto* slider = new juce::Slider(juce::Slider::RotaryHorizontalVerticalDrag,
                                      juce::Slider::TextBoxBelow);
        
        slider->setRange(paramRanges[i * 3],     // minimum
                        paramRanges[i * 3 + 1],   // maximum
                        (paramRanges[i * 3 + 1] - paramRanges[i * 3]) / 100.0f); // interval
        slider->setValue(paramRanges[i * 3 + 2], juce::dontSendNotification); // default
        slider->setTextValueSuffix(" " + paramLabels[i]);
        slider->setPopupDisplayEnabled(true, false, this);
        slider->setColour(juce::Slider::thumbColourId, juce::Colours::orange);
        slider->setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::orange);
        slider->setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
        
        auto* label = new juce::Label();
        label->setText(paramNames[i], juce::dontSendNotification);
        label->setJustificationType(juce::Justification::centred);
        
        sliders.add(slider);
        labels.add(label);
        
        slider->onValueChange = [this, effect, i]
        {
            effect->setParameter(i, static_cast<float>(sliders[i]->getValue()));
        };
        
        addAndMakeVisible(slider);
        addAndMakeVisible(label);
    }

    // ドラッグ＆ドロップの設定
    setMouseCursor(juce::MouseCursor::DraggingHandCursor);
}

EffectComponent::~EffectComponent()
{
    sliders.clear(true);
    labels.clear(true);
}

void EffectComponent::paint(juce::Graphics& g)
{
    // 背景の描画
    auto bounds = getLocalBounds().toFloat();
    g.setColour(juce::Colours::darkgrey.darker(0.2f));
    g.fillRoundedRectangle(bounds, 10.0f);
    
    // 枠線の描画
    g.setColour(juce::Colours::orange.withAlpha(0.5f));
    g.drawRoundedRectangle(bounds.reduced(1.0f), 10.0f, 2.0f);

    // ドラッグ中の表示
    if (isBeingDragged)
    {
        g.setColour(juce::Colours::white.withAlpha(0.2f));
        g.fillRoundedRectangle(bounds, 10.0f);
    }
}

void EffectComponent::resized()
{
    auto bounds = getLocalBounds().reduced(10);
    
    // ヘッダー部分
    auto headerBounds = bounds.removeFromTop(30);
    removeButton.setBounds(headerBounds.removeFromRight(30).reduced(2));
    bypassButton.setBounds(headerBounds.removeFromRight(60).reduced(2));
    titleLabel.setBounds(headerBounds);

    // パラメーターコントロール
    const int numParams = effect->getNumParameters();
    const int sliderWidth = bounds.getWidth() / 2;
    const int sliderHeight = (bounds.getHeight() - 20) / ((numParams + 1) / 2);

    for (int i = 0; i < numParams; ++i)
    {
        const int row = i / 2;
        const int col = i % 2;
        
        auto sliderBounds = bounds.withTrimmedTop(row * sliderHeight)
                                 .withTrimmedLeft(col * sliderWidth)
                                 .withWidth(sliderWidth)
                                 .withHeight(sliderHeight)
                                 .reduced(5);
        
        auto labelBounds = sliderBounds.removeFromTop(20);
        labels[i]->setBounds(labelBounds);
        sliders[i]->setBounds(sliderBounds);
    }
}

void EffectComponent::mouseDown(const juce::MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
        dragger.startDraggingComponent(this, e);
}

void EffectComponent::mouseDrag(const juce::MouseEvent& e)
{
    if (e.mods.isLeftButtonDown())
    {
        dragger.dragComponent(this, e, nullptr);
        isBeingDragged = true;
        repaint();
    }
}

void EffectComponent::mouseUp(const juce::MouseEvent&)
{
    isBeingDragged = false;
    repaint();
}

bool EffectComponent::isInterestedInDragSource(const SourceDetails&)
{
    return true;
}

void EffectComponent::itemDragEnter(const SourceDetails&)
{
    repaint();
}

void EffectComponent::itemDragMove(const SourceDetails&)
{
    repaint();
}

void EffectComponent::itemDragExit(const SourceDetails&)
{
    repaint();
}

void EffectComponent::itemDropped(const SourceDetails& dragSourceDetails)
{
    if (auto* draggedEffect = dynamic_cast<EffectComponent*>(dragSourceDetails.sourceComponent.get()))
    {
        if (auto* parent = dynamic_cast<DragAndDropInterface*>(getParentComponent()))
        {
            const int fromIndex = draggedEffect->getIndexInParent();
            const int toIndex = getIndexInParent();
            parent->handleEffectDragAndDrop(fromIndex, toIndex);
        }
    }
}

void EffectComponent::updateParameters()
{
    for (int i = 0; i < sliders.size(); ++i)
    {
        sliders[i]->setValue(effect->getParameter(i), juce::dontSendNotification);
    }
}

void EffectComponent::parameterChanged(int parameterIndex, float newValue)
{
    if (parameterIndex >= 0 && parameterIndex < sliders.size())
    {
        sliders[parameterIndex]->setValue(newValue, juce::dontSendNotification);
    }
}

int EffectComponent::getIndexInParent() const
{
    if (auto* parent = getParentComponent())
    {
        for (int i = 0; i < parent->getNumChildComponents(); ++i)
        {
            if (parent->getChildComponent(i) == this)
            {
                return i;
            }
        }
    }
    return -1;
} 