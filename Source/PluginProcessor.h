#pragma once
#include <JuceHeader.h>
#include "GoaSynth.h"

class GoaGaterProcessor : public juce::AudioProcessor
{
public:
    GoaGaterProcessor();
    ~GoaGaterProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Goa Gater"; }
    bool acceptsMidi() const override  { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    int getNumPrograms() override;
    int getCurrentProgram() override { return currentProgram; }
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void applyPreset (int index);
    void setP (const juce::String& id, float value);

    GoaParams p;
    juce::Synthesiser synth;
    std::atomic<float> *gateOn, *gateRate, *gateDepth, *gateSmooth;
    std::atomic<float>* stepParam[16];

    int currentProgram = 0;
    double sampleRate = 44100.0, freePpq = 0.0;
    float gateGain = 1.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GoaGaterProcessor)
};
