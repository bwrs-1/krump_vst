#pragma once

#include "Effect.h"
#include <juce_dsp/juce_dsp.h>

/**
 * SP-404スタイルのリバーブエフェクト
 * - ルームサイズ
 * - ダンピング
 * - ウェット/ドライミックス
 */
class ReverbEffect : public Effect
{
public:
    ReverbEffect();
    ~ReverbEffect() override = default;

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
    juce::String getName() const override { return "Reverb"; }
    juce::String getCategory() const override { return "Reverb"; }
    int getNumParameters() const override { return 2; }

    // プリセット関連
    void saveToXml(juce::XmlElement& xml) const override;
    void loadFromXml(const juce::XmlElement& xml) override;

private:
    float roomSize = 0.5f;  // ルームサイズ (0-1)
    float damping = 0.5f;   // ダンピング (0-1)

    juce::dsp::Reverb reverb;
    void updateReverbParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbEffect)
}; 