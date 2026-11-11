#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class GoaGaterEditor : public juce::AudioProcessorEditor
{
public:
    explicit GoaGaterEditor (GoaGaterProcessor&);
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SA = juce::AudioProcessorValueTreeState::SliderAttachment;
    using BA = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using CA = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    struct Knob { juce::Slider s; juce::Label l; std::unique_ptr<SA> a; };

    GoaGaterProcessor& proc;
    juce::ComboBox presetBox, waveBox, rateBox;
    juce::ToggleButton gateOn { "Gate" };
    std::vector<std::unique_ptr<Knob>> knobs;
    std::array<juce::TextButton, 16> steps;
    std::unique_ptr<CA> waveA, rateA;
    std::unique_ptr<BA> gateA;
    std::vector<std::unique_ptr<BA>> stepA;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GoaGaterEditor)
};
