#pragma once

#include "Effect.h"
#include <juce_dsp/juce_dsp.h>

/**
 * フィルターエフェクト
 * - ローパス
 * - ハイパス
 * - バンドパス
 */
class FilterEffect : public Effect
{
public:
    FilterEffect();
    ~FilterEffect() override = default;

    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;

    // パラメータ関連
    juce::StringArray getParameterNames() const override;
    juce::StringArray getParameterLabels() const override;
    juce::Array<float> getParameterRanges() const override;
    void setParameter(int parameterIndex, float value) override;
    float getParameter(int parameterIndex) const override;

    // エフェクト情報
    juce::String getName() const override { return "Filter"; }
    juce::String getCategory() const override { return "Filter"; }
    int getNumParameters() const override { return 2; }

    // プリセット関連
    void saveToXml(juce::XmlElement& xml) const override;
    void loadFromXml(const juce::XmlElement& xml) override;

private:
    float cutoff = 1000.0f;  // カットオフ周波数 (20Hz - 20kHz)
    float resonance = 0.7f;  // レゾナンス (0.1 - 8.0)

    juce::dsp::StateVariableTPTFilter<float> filter;
    void updateFilterParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterEffect)
}; 