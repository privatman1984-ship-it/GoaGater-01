#pragma once
#include <JuceHeader.h>

struct GoaParams
{
    std::atomic<float> *wave, *uni, *det, *sub, *cutoff, *res, *envAmt, *fdec,
                       *att, *dec, *sus, *rel, *gain;
};

struct GoaSound : public juce::SynthesiserSound
{
    bool appliesToNote (int) override    { return true; }
    bool appliesToChannel (int) override { return true; }
};

inline float polyBlep (float t, float dt)
{
    if (t < dt)         { t /= dt;           return t + t - t * t - 1.0f; }
    if (t > 1.0f - dt)  { t = (t - 1.0f) / dt; return t * t + t + t + 1.0f; }
    return 0.0f;
}

class GoaVoice : public juce::SynthesiserVoice
{
public:
    explicit GoaVoice (const GoaParams& p) : prm (p) {}

    bool canPlaySound (juce::SynthesiserSound* s) override
    {
        return dynamic_cast<GoaSound*> (s) != nullptr;
    }

    void setCurrentPlaybackSampleRate (double sr) override
    {
        juce::SynthesiserVoice::setCurrentPlaybackSampleRate (sr);
        ampEnv.setSampleRate (sr);
        juce::dsp::ProcessSpec spec { sr, 512, 1 };
        for (auto& f : filt)
        {
            f.prepare (spec);
            f.setType (juce::dsp::StateVariableTPTFilterType::lowpass);
        }
    }

    void startNote (int note, float vel, juce::SynthesiserSound*, int) override
    {
        baseFreq = (float) juce::MidiMessage::getMidiNoteInHertz (note);
        velocity = vel;
        fEnv = 1.0f;
        for (int i = 0; i < 7; ++i) phase[i] = (i == 0) ? 0.0f : random.nextFloat();
        subPhase = 0.0f;
        for (auto& f : filt) f.reset();
        ampEnv.noteOn();
    }

    void stopNote (float, bool allowTailOff) override
    {
        if (allowTailOff) ampEnv.noteOff();
        else { ampEnv.reset(); clearCurrentNote(); }
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& out, int start, int num) override
    {
        if (! isVoiceActive()) return;

        const float sr = (float) getSampleRate();
        ampEnv.setParameters ({ prm.att->load(), prm.dec->load(), prm.sus->load(), prm.rel->load() });

        const int   n       = juce::jlimit (1, 7, (int) prm.uni->load());
        const float detC    = prm.det->load() * 40.0f;
        const bool  square  = prm.wave->load() > 0.5f;
        const float subLvl  = prm.sub->load();
        const float cutBase = prm.cutoff->load();
        const float res     = prm.res->load();
        const float q       = 0.6f + res * res * 11.0f;
        const float envAmt  = prm.envAmt->load();
        const float fCoef   = std::exp (-1.0f / (std::max (0.005f, prm.fdec->load()) * sr));
        const float amp0    = prm.gain->load() * 0.35f;

        float inc[7], pan[7];
        for (int u = 0; u < n; ++u)
        {
            const float off = n > 1 ? (2.0f * (float) u / (float) (n - 1) - 1.0f) : 0.0f;
            inc[u] = baseFreq * std::pow (2.0f, off * detC / 1200.0f) / sr;
            pan[u] = 0.5f + 0.4f * off;
        }
        const float norm   = 2.0f / std::sqrt ((float) n);
        const float subInc = baseFreq * 0.5f / sr;

        for (int i = 0; i < num; ++i)
        {
            if ((i & 7) == 0)
            {
                const float fc = std::min (18000.0f, cutBase * std::pow (2.0f, envAmt * fEnv * 5.0f));
                for (auto& f : filt) { f.setCutoffFrequency (fc); f.setResonance (q); }
            }
            fEnv *= fCoef;

            const float a = ampEnv.getNextSample();
            if (! ampEnv.isActive()) { clearCurrentNote(); break; }

            float l = 0.0f, r = 0.0f;
            for (int u = 0; u < n; ++u)
            {
                float& ph = phase[u];
                float s;
                if (square)
                {
                    float t2 = ph + 0.5f; if (t2 >= 1.0f) t2 -= 1.0f;
                    s = (ph < 0.5f ? 1.0f : -1.0f) + polyBlep (ph, inc[u]) - polyBlep (t2, inc[u]);
                }
                else
                    s = 2.0f * ph - 1.0f - polyBlep (ph, inc[u]);

                l += s * (1.0f - pan[u]);
                r += s * pan[u];
                ph += inc[u]; if (ph >= 1.0f) ph -= 1.0f;
            }
            l *= norm; r *= norm;

            const float sb = std::sin (juce::MathConstants<float>::twoPi * subPhase) * subLvl * 0.7f;
            subPhase += subInc; if (subPhase >= 1.0f) subPhase -= 1.0f;
            l += sb; r += sb;

            l = filt[0].processSample (0, l);
            r = filt[1].processSample (0, r);

            const float amp = a * velocity * amp0;
            out.addSample (0, start + i, l * amp);
            if (out.getNumChannels() > 1) out.addSample (1, start + i, r * amp);
        }
    }

private:
    const GoaParams& prm;
    juce::ADSR ampEnv;
    juce::dsp::StateVariableTPTFilter<float> filt[2];
    juce::Random random;
    float phase[7] {}, subPhase = 0.0f, baseFreq = 440.0f, velocity = 1.0f, fEnv = 1.0f;
};
