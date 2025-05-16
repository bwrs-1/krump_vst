#include "PitchEffect.h"

PitchEffect::PitchEffect()
{
    updatePitchShift();
}

void PitchEffect::prepare(const juce::dsp::ProcessSpec& spec)
{
    lastSampleRate = static_cast<float>(spec.sampleRate);
    bufferLength = static_cast<int>(2.0f * lastSampleRate); // 2秒のバッファ
    pitchBuffer.setSize(spec.numChannels, bufferLength);
    pitchBuffer.clear();
    writePosition = 0;
    updatePitchShift();
}

void PitchEffect::process(juce::AudioBuffer<float>& buffer)
{
    if (!isEnabled)
        return;

    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();
    const float pitchRatio = getPitchRatio();

    for (int channel = 0; channel < numChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer(channel);
        float* pitchData = pitchBuffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float in = channelData[sample];
            float readPosition = writePosition - (static_cast<float>(bufferLength) / pitchRatio);
            
            if (readPosition < 0)
                readPosition += bufferLength;

            // 線形補間
            const int readPos1 = static_cast<int>(readPosition);
            const int readPos2 = (readPos1 + 1) % bufferLength;
            const float fraction = readPosition - readPos1;
            
            const float out = pitchData[readPos1] + fraction * (pitchData[readPos2] - pitchData[readPos1]);
            
            pitchData[writePosition] = in;
            channelData[sample] = out;

            writePosition = (writePosition + 1) % bufferLength;
        }
    }
}

void PitchEffect::reset()
{
    pitchBuffer.clear();
    writePosition = 0;
}

void PitchEffect::updatePitchShift()
{
    pitch = juce::jlimit(-12.0f, 12.0f, pitch);
    fineTune = juce::jlimit(-50.0f, 50.0f, fineTune);
}

float PitchEffect::getPitchRatio() const
{
    // 半音とセントの変換
    const float semitones = pitch + (fineTune / 100.0f);
    return std::pow(2.0f, semitones / 12.0f);
}

juce::StringArray PitchEffect::getParameterNames() const
{
    return {"Pitch", "Fine"};
}

juce::StringArray PitchEffect::getParameterLabels() const
{
    return {"st", "cent"};
}

juce::Array<float> PitchEffect::getParameterRanges() const
{
    return {
        -12.0f, 12.0f, 0.0f,  // Pitch: -12 to +12 半音
        -50.0f, 50.0f, 0.0f   // Fine: -50 to +50 セント
    };
}

void PitchEffect::setParameter(int parameterIndex, float value)
{
    switch (parameterIndex)
    {
        case 0:
            pitch = value;
            break;
        case 1:
            fineTune = value;
            break;
        default:
            return;
    }
    updatePitchShift();
}

float PitchEffect::getParameter(int parameterIndex) const
{
    switch (parameterIndex)
    {
        case 0: return pitch;
        case 1: return fineTune;
        default: return 0.0f;
    }
}

void PitchEffect::saveToXml(juce::XmlElement& xml) const
{
    xml.setAttribute("Pitch", pitch);
    xml.setAttribute("FineTune", fineTune);
}

void PitchEffect::loadFromXml(const juce::XmlElement& xml)
{
    pitch = static_cast<float>(xml.getDoubleAttribute("Pitch", pitch));
    fineTune = static_cast<float>(xml.getDoubleAttribute("FineTune", fineTune));
    updatePitchShift();
} 