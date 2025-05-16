#pragma once

#include "Effect.h"
#include <juce_dsp/juce_dsp.h>

/**
 * SP-404スタイルのディレイエフェクト
 * - タイムディビジョン（1/4, 1/8, 1/16など）
 * - フィードバック
 * - ウェット/ドライミックス
 * - ピッチシフト
 */
class DelayEffect : public Effect
{
public:
    // ディレイタイプの列挙型
    enum DelayType
    {
        DIGITAL, // デジタルディレイ
        ANALOG,  // アナログディレイエミュレーション
        TAPE,    // テープディレイエミュレーション
        NUM_TYPES
    };

    DelayEffect();
    ~DelayEffect() override = default;

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
    juce::String getName() const override { return "Delay"; }
    juce::String getCategory() const override { return "Time"; }
    int getNumParameters() const override { return 2; }

    // プリセット関連
    void saveToXml(juce::XmlElement& xml) const override;
    void loadFromXml(const juce::XmlElement& xml) override;

private:
    float feedback = 0.5f;
    float time = 0.5f;

    juce::AudioBuffer<float> delayBuffer;
    int delayBufferLength = 0;
    int writePosition = 0;
    float lastSampleRate = 44100.0f;

    void updateDelayTime();

    // ディレイタイプごとの処理
    void processDigitalDelay(float* channelData, float* delayData, 
                           int bufferLength, int channel);
    void processAnalogDelay(float* channelData, float* delayData, 
                          int bufferLength, int channel);
    void processTapeDelay(float* channelData, float* delayData, 
                         int bufferLength, int channel);

    // ユーティリティ関数
    float getInterpolatedSample(const float* buffer, float position, int bufferLength);
    float hermiteInterpolate(float y0, float y1, float y2, float y3, float t);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DelayEffect)
}; 