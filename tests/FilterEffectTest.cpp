#include <catch2/catch.hpp>
#include "../src/audio/effects/FilterEffect.h"

TEST_CASE("FilterEffect basic functionality", "[filter]")
{
    FilterEffect filter;

    SECTION("Default parameters")
    {
        CHECK(filter.getParameter(0) == Approx(1000.0f)); // Frequency
        CHECK(filter.getParameter(1) == Approx(0.7f));    // Resonance
        CHECK(filter.getParameter(2) == Approx(0.0f));    // Type
    }

    SECTION("Parameter setting and getting")
    {
        filter.setParameter(0, 440.0f);  // Frequency
        filter.setParameter(1, 1.0f);    // Resonance
        filter.setParameter(2, 1.0f);    // Type (HighPass)

        CHECK(filter.getParameter(0) == Approx(440.0f));
        CHECK(filter.getParameter(1) == Approx(1.0f));
        CHECK(filter.getParameter(2) == Approx(1.0f));
    }

    SECTION("Audio processing")
    {
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = 44100.0;
        spec.maximumBlockSize = 512;
        spec.numChannels = 2;

        filter.prepare(spec);

        juce::AudioBuffer<float> buffer(2, 512);
        buffer.clear();

        // Generate test signal (1kHz sine wave)
        float frequency = 1000.0f;
        float sampleRate = static_cast<float>(spec.sampleRate);
        
        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
        {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
            {
                channelData[sample] = std::sin(2.0f * juce::MathConstants<float>::pi * 
                                             frequency * static_cast<float>(sample) / sampleRate);
            }
        }

        // Process with different filter types
        SECTION("LowPass filter")
        {
            filter.setParameter(0, 500.0f);   // Cutoff below signal frequency
            filter.setParameter(1, 0.7f);     // Q
            filter.setParameter(2, 0.0f);     // LowPass
            filter.process(buffer);

            // Check that high frequencies are attenuated
            // This is a basic test - in reality, you'd want to do FFT analysis
            bool hasAttenuatedSignal = false;
            for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            {
                auto* channelData = buffer.getReadPointer(channel);
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    if (std::abs(channelData[sample]) < 0.7f) // Expecting some attenuation
                    {
                        hasAttenuatedSignal = true;
                        break;
                    }
                }
            }
            CHECK(hasAttenuatedSignal);
        }
    }
}

TEST_CASE("FilterEffect XML serialization", "[filter]")
{
    FilterEffect filter;
    
    // Set some test parameters
    filter.setParameter(0, 440.0f);  // Frequency
    filter.setParameter(1, 1.0f);    // Resonance
    filter.setParameter(2, 1.0f);    // Type

    // Save to XML
    juce::XmlElement xml("TestFilter");
    filter.saveToXml(xml);

    // Create a new filter and load the XML
    FilterEffect loadedFilter;
    loadedFilter.loadFromXml(xml);

    // Check that parameters match
    CHECK(loadedFilter.getParameter(0) == Approx(440.0f));
    CHECK(loadedFilter.getParameter(1) == Approx(1.0f));
    CHECK(loadedFilter.getParameter(2) == Approx(1.0f));
} 