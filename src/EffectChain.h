#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Effect
{
public:
    Effect() = default;
    virtual ~Effect() = default;

    virtual void prepare(const juce::dsp::ProcessSpec& spec) = 0;
    virtual void process(juce::AudioBuffer<float>& buffer) = 0;
    virtual void reset() = 0;

    void setEnabled(bool shouldBeEnabled) { isEnabled = shouldBeEnabled; }
    bool getEnabled() const { return isEnabled; }

    virtual juce::StringArray getParameterNames() const = 0;
    virtual void setParameter(int parameterIndex, float value) = 0;
    virtual float getParameter(int parameterIndex) const = 0;

protected:
    bool isEnabled = false;
};

class DelayEffect : public Effect
{
public:
    DelayEffect();
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    
    juce::StringArray getParameterNames() const override;
    void setParameter(int parameterIndex, float value) override;
    float getParameter(int parameterIndex) const override;

private:
    float time = 0.5f;
    float feedback = 0.3f;
    float mix = 0.5f;
    
    juce::AudioBuffer<float> delayBuffer;
    int delayBufferLength = 0;
    int writePosition = 0;
    float sampleRate = 44100.0f;
};

class FilterEffect : public Effect
{
public:
    FilterEffect();
    void prepare(const juce::dsp::ProcessSpec& spec) override;
    void process(juce::AudioBuffer<float>& buffer) override;
    void reset() override;
    
    juce::StringArray getParameterNames() const override;
    void setParameter(int parameterIndex, float value) override;
    float getParameter(int parameterIndex) const override;

private:
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, 
                                  juce::dsp::IIR::Coefficients<float>> filter;
    float frequency = 1000.0f;
    float resonance = 0.7f;
    int type = 0; // 0: LowPass, 1: HighPass, 2: BandPass
    float sampleRate = 44100.0f;

    void updateFilter();
};

class EffectChain
{
public:
    EffectChain();
    
    void prepare(const juce::dsp::ProcessSpec& spec);
    void process(juce::AudioBuffer<float>& buffer);
    void reset();

    void addEffect(std::unique_ptr<Effect> effect);
    void removeEffect(int index);
    void moveEffect(int fromIndex, int toIndex);
    
    Effect* getEffect(int index);
    int getNumEffects() const;

private:
    std::vector<std::unique_ptr<Effect>> effects;
    juce::dsp::ProcessSpec currentSpec;
}; 