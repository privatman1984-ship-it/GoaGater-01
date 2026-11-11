#include "PluginEditor.h"
#include "Presets.h"

using namespace juce;

GoaGaterEditor::GoaGaterEditor (GoaGaterProcessor& p) : AudioProcessorEditor (&p), proc (p)
{
    setSize (760, 420);

    for (int i = 0; i < numGoaPresets; ++i) presetBox.addItem (goaPresets[i].name, i + 1);
    presetBox.setSelectedId (proc.getCurrentProgram() + 1, dontSendNotification);
    presetBox.onChange = [this] { proc.setCurrentProgram (presetBox.getSelectedId() - 1); };
    addAndMakeVisible (presetBox);

    waveBox.addItemList ({ "Saw", "Square" }, 1);
    rateBox.addItemList ({ "1/4", "1/8", "1/16", "1/32" }, 1);
    addAndMakeVisible (waveBox); addAndMakeVisible (rateBox); addAndMakeVisible (gateOn);
    waveA = std::make_unique<CA> (proc.apvts, "wave", waveBox);
    rateA = std::make_unique<CA> (proc.apvts, "gateRate", rateBox);
    gateA = std::make_unique<BA> (proc.apvts, "gateOn", gateOn);

    struct Spec { const char* id; const char* label; };
    const Spec specs[] = {
        { "uni", "Unison" }, { "det", "Detune" }, { "sub", "Sub" }, { "cutoff", "Cutoff" },
        { "res", "Reso" }, { "envAmt", "Filt Env" }, { "fdec", "Filt Dec" },
        { "att", "Attack" }, { "dec", "Decay" }, { "sus", "Sustain" }, { "rel", "Release" },
        { "gain", "Volume" }, { "gateDepth", "Gate Depth" }, { "gateSmooth", "Gate Smooth" } };

    for (auto& sp : specs)
    {
        auto k = std::make_unique<Knob>();
        k->s.setSliderStyle (Slider::RotaryHorizontalVerticalDrag);
        k->s.setTextBoxStyle (Slider::TextBoxBelow, false, 70, 16);
        k->l.setText (sp.label, dontSendNotification);
        k->l.setJustificationType (Justification::centred);
        addAndMakeVisible (k->s); addAndMakeVisible (k->l);
        k->a = std::make_unique<SA> (proc.apvts, sp.id, k->s);
        knobs.push_back (std::move (k));
    }

    for (int i = 0; i < 16; ++i)
    {
        steps[(size_t) i].setButtonText (String (i + 1));
        steps[(size_t) i].setClickingTogglesState (true);
        steps[(size_t) i].setColour (TextButton::buttonOnColourId, Colour (0xffff7a00));
        addAndMakeVisible (steps[(size_t) i]);
        stepA.push_back (std::make_unique<BA> (proc.apvts, "step" + String (i + 1), steps[(size_t) i]));
    }
}

void GoaGaterEditor::paint (Graphics& g)
{
    g.setGradientFill (ColourGradient (Colour (0xff1a0033), 0, 0, Colour (0xff002b1f), 0, (float) getHeight(), false));
    g.fillAll();
    g.setColour (Colour (0xff7CFF4F));
    g.setFont (Font (FontOptions (22.0f, Font::bold)));
    g.drawText ("GOA GATER", 10, 8, 160, 30, Justification::centredLeft);
}

void GoaGaterEditor::resized()
{
    auto area = getLocalBounds().reduced (10);
    auto top = area.removeFromTop (30);
    top.removeFromLeft (170);
    presetBox.setBounds (top.removeFromLeft (230)); top.removeFromLeft (10);
    waveBox.setBounds (top.removeFromLeft (90));    top.removeFromLeft (10);
    rateBox.setBounds (top.removeFromLeft (80));    top.removeFromLeft (10);
    gateOn.setBounds (top.removeFromLeft (80));

    area.removeFromTop (10);
    const int w = area.getWidth() / 7, rowH = 140;
    for (size_t i = 0; i < knobs.size(); ++i)
    {
        const int col = (int) i % 7, row = (int) i / 7;
        Rectangle<int> r (area.getX() + col * w, area.getY() + row * rowH, w, rowH);
        knobs[i]->l.setBounds (r.removeFromTop (18));
        knobs[i]->s.setBounds (r.reduced (4));
    }

    auto stepRow = area.withTrimmedTop (2 * rowH + 6).removeFromTop (36);
    const int sw = stepRow.getWidth() / 16;
    for (int i = 0; i < 16; ++i)
        steps[(size_t) i].setBounds (stepRow.getX() + i * sw, stepRow.getY(), sw - 3, stepRow.getHeight());
}
