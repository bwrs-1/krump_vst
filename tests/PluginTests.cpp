#include <JuceHeader.h>
#include "../src/PluginProcessor.h"

class KrumpVSTTests : public juce::UnitTest
{
public:
    KrumpVSTTests() : UnitTest("KrumpVST Tests") {}

    void runTest() override
    {
        beginTest("Plugin Initialization");
        {
            KrumpVSTAudioProcessor processor;
            expect(processor.getName() == "KrumpVST");
        }

        beginTest("Audio Processing");
        {
            KrumpVSTAudioProcessor processor;
            juce::AudioBuffer<float> buffer(2, 512);
            juce::MidiBuffer midiBuffer;
            
            processor.prepareToPlay(44100.0, 512);
            processor.processBlock(buffer, midiBuffer);
            processor.releaseResources();
        }

        beginTest("Parameter Handling");
        {
            KrumpVSTAudioProcessor processor;
            // Test parameter ranges and values
            expect(processor.getParameter(0) >= 0.0f && processor.getParameter(0) <= 1.0f);
        }
    }
};

static KrumpVSTTests krumpVSTTests; 