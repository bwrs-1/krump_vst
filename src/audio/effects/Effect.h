#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

/**
 * エフェクトの基本クラス
 * すべてのエフェクトはこのクラスを継承して実装します
 */
class Effect
{
public:
    Effect() = default;
    virtual ~Effect() = default;

    // オーディオ処理関連
    virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;
    virtual void reset() = 0;

    // エフェクトの有効/無効
    void setEnabled(bool shouldBeEnabled) { isEnabled = shouldBeEnabled; }
    bool getEnabled() const { return isEnabled; }

    // パラメータ関連
    virtual juce::StringArray getParameterNames() const = 0;
    virtual juce::StringArray getParameterLabels() const = 0;  // dB, Hz, ms など
    virtual juce::Array<float> getParameterRanges() const = 0; // min, max, default
    virtual void setParameter(int parameterIndex, float value) = 0;
    virtual float getParameter(int parameterIndex) const = 0;

    // エフェクトの識別情報
    virtual juce::String getName() const = 0;
    virtual juce::String getCategory() const = 0;
    virtual int getNumParameters() const = 0;

    // プリセット関連
    virtual void saveToXml(juce::XmlElement& xml) const = 0;
    virtual void loadFromXml(const juce::XmlElement& xml) = 0;

    bool isEnabled = true;

protected:
    float sampleRate = 44100.0f;
    int blockSize = 512;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Effect)
}; 