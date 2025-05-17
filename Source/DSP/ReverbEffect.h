#pragma once

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * SP-404スタイルのリバーブエフェクト
 * - ルームサイズ
 * - ダンピング
 * - ウェット/ドライミックス
 */
class ReverbEffect
{
public:
    ReverbEffect() = default;
    ~ReverbEffect();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void processBlock(juce::AudioBuffer<float>& buffer);
    void reset();

    // Parameter setters
    void setRoomSize(float value);
    void setDamping(float value);
    void setWetLevel(float value);
    void setDryLevel(float value);
    void setWidth(float value);
    void setFreezeMode(bool value);

    // パラメータ関連
    juce::StringArray getParameterNames() const;
    juce::StringArray getParameterLabels() const;
    juce::Array<float> getParameterRanges() const;
    void setParameter(int parameterIndex, float value);
    float getParameter(int parameterIndex) const;

    // エフェクト情報
    juce::String getName() const { return "Reverb"; }
    juce::String getCategory() const { return "Reverb"; }
    int getNumParameters() const { return 5; }

    // プリセット関連
    void saveToXml(juce::XmlElement& xml) const;
    void loadFromXml(const juce::XmlElement& xml);

    void setParameters(const juce::Reverb::Parameters& params) { reverb.setParameters(params); }
    juce::Reverb::Parameters getParameters() const { return reverb.getParameters(); }

private:
    juce::Reverb reverb;
    juce::dsp::Reverb::Parameters parameters;
    void updateParameters();
}; 