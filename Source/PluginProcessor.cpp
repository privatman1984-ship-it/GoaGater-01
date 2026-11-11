#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Presets.h"

using namespace juce;

AudioProcessorValueTreeState::ParameterLayout GoaGaterProcessor::createParameterLayout()
{
    using NR = NormalisableRange<float>;
    auto pid = [] (const String& s) { return ParameterID { s, 1 }; };
    AudioProcessorValueTreeState::ParameterLayout l;

    l.add (std::make_unique<AudioParameterChoice> (pid ("wave"), "Wave", StringArray { "Saw", "Square" }, 0));
    l.add (std::make_unique<AudioParameterInt>    (pid ("uni"),  "Unison", 1, 7, 5));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("det"),  "Detune",  NR (0.f, 1.f), 0.3f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("sub"),  "Sub",     NR (0.f, 1.f), 0.f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("cutoff"), "Cutoff", NR (40.f, 18000.f, 0.f, 0.3f), 2000.f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("res"),  "Reso",    NR (0.f, 1.f), 0.3f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("envAmt"), "Filt Env", NR (0.f, 1.f), 0.5f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("fdec"), "Filt Dec", NR (0.01f, 1.5f, 0.f, 0.5f), 0.2f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("att"),  "Attack",  NR (0.001f, 2.f, 0.f, 0.4f), 0.005f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("dec"),  "Decay",   NR (0.01f, 2.f, 0.f, 0.4f), 0.2f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("sus"),  "Sustain", NR (0.f, 1.f), 0.8f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("rel"),  "Release", NR (0.01f, 4.f, 0.f, 0.4f), 0.25f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("gain"), "Volume",  NR (0.f, 1.f), 0.8f));

    l.add (std::make_unique<AudioParameterBool>   (pid ("gateOn"), "Gate", true));
    l.add (std::make_unique<AudioParameterChoice> (pid ("gateRate"), "Gate Rate", StringArray { "1/4", "1/8", "1/16", "1/32" }, 2));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("gateDepth"), "Gate Depth", NR (0.f, 1.f), 1.f));
    l.add (std::make_unique<AudioParameterFloat>  (pid ("gateSmooth"), "Gate Smooth", NR (0.f, 1.f), 0.2f));
    for (int i = 0; i < 16; ++i)
        l.add (std::make_unique<AudioParameterBool> (pid ("step" + String (i + 1)), "Step " + String (i + 1), i % 2 == 0));
    return l;
}

GoaGaterProcessor::GoaGaterProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createParameterLayout())
{
    auto g = [this] (const char* id) { return apvts.getRawParameterValue (id); };
    p = { g ("wave"), g ("uni"), g ("det"), g ("sub"), g ("cutoff"), g ("res"), g ("envAmt"),
          g ("fdec"), g ("att"), g ("dec"), g ("sus"), g ("rel"), g ("gain") };
    gateOn = g ("gateOn"); gateRate = g ("gateRate");
    gateDepth = g ("gateDepth"); gateSmooth = g ("gateSmooth");
    for (int i = 0; i < 16; ++i) stepParam[i] = apvts.getRawParameterValue ("step" + String (i + 1));

    for (int i = 0; i < 12; ++i) synth.addVoice (new GoaVoice (p));
    synth.addSound (new GoaSound());
}

void GoaGaterProcessor::prepareToPlay (double sr, int)
{
    sampleRate = sr;
    synth.setCurrentPlaybackSampleRate (sr);
    gateGain = 1.0f;
}

bool GoaGaterProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    return l.getMainOutputChannelSet() == AudioChannelSet::stereo();
}

void GoaGaterProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midi)
{
    ScopedNoDenormals noDenormals;
    const int n = buffer.getNumSamples();
    buffer.clear();
    synth.renderNextBlock (buffer, midi, 0, n);

    if (gateOn->load() < 0.5f) return;

    double bpm = 140.0, ppq = freePpq;
    bool hostPos = false;
    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
        {
            if (auto b = pos->getBpm()) bpm = *b;
            if (pos->getIsPlaying())
                if (auto pp = pos->getPpqPosition()) { ppq = *pp; hostPos = true; }
        }

    static const double stepsPerBeat[] = { 1.0, 2.0, 4.0, 8.0 };
    const double spb      = stepsPerBeat[jlimit (0, 3, (int) gateRate->load())];
    const double ppqInc   = bpm / 60.0 / sampleRate;
    const float  depth    = gateDepth->load();
    const float  tau      = 0.0005f + gateSmooth->load() * 0.02f;
    const float  coef     = 1.0f - std::exp (-1.0f / ((float) sampleRate * tau));

    for (int i = 0; i < n; ++i)
    {
        const int idx = (((int) std::floor (ppq * spb)) % 16 + 16) % 16;
        const float target = stepParam[idx]->load() > 0.5f ? 1.0f : 1.0f - depth;
        gateGain += coef * (target - gateGain);
        for (int c = 0; c < buffer.getNumChannels(); ++c)
            buffer.getWritePointer (c)[i] *= gateGain;
        ppq += ppqInc;
    }
    freePpq = hostPos ? 0.0 : ppq;
}

void GoaGaterProcessor::setP (const String& id, float v)
{
    if (auto* prm = apvts.getParameter (id))
        prm->setValueNotifyingHost (prm->convertTo0to1 (v));
}

void GoaGaterProcessor::applyPreset (int i)
{
    if (i < 0 || i >= numGoaPresets) return;
    const auto& s = goaPresets[i];
    setP ("wave", (float) s.wave);   setP ("uni", (float) s.uni);
    setP ("det", s.det);             setP ("sub", s.sub);
    setP ("cutoff", s.cutoff);       setP ("res", s.res);
    setP ("envAmt", s.envAmt);       setP ("fdec", s.fdec);
    setP ("att", s.att);             setP ("dec", s.dec);
    setP ("sus", s.sus);             setP ("rel", s.rel);
    setP ("gateRate", (float) s.rate);
    setP ("gateDepth", s.depth);     setP ("gateSmooth", s.smooth);
    setP ("gateOn", 1.0f);
    for (int k = 0; k < 16; ++k)
        setP ("step" + String (k + 1), s.pattern[k] == 'x' ? 1.0f : 0.0f);
}

int GoaGaterProcessor::getNumPrograms() { return numGoaPresets; }
void GoaGaterProcessor::setCurrentProgram (int i) { currentProgram = i; applyPreset (i); }
const String GoaGaterProcessor::getProgramName (int i)
{
    return (i >= 0 && i < numGoaPresets) ? String (goaPresets[i].name) : String();
}

void GoaGaterProcessor::getStateInformation (MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml()) copyXmlToBinary (*xml, dest);
}

void GoaGaterProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
            apvts.replaceState (ValueTree::fromXml (*xml));
}

AudioProcessorEditor* GoaGaterProcessor::createEditor() { return new GoaGaterEditor (*this); }

AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new GoaGaterProcessor(); }
