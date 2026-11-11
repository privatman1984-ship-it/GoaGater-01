# Goa Gater

Goa-Trance Synth (VSTi / VST3 + Standalone) mit eingebautem 16-Step Trance-Gate und 16 Goa-Presets.

- 2-fach Unison-Saw/Square (bis 7 Stimmen) + Sub, Resonanz-Lowpass mit Filter-Hüllkurve
- Tempo-synchroner Gater (1/4, 1/8, 1/16, 1/32), 16 Steps, Depth + Smooth
- Presets: Goa Classic Gate, Psy Rolling Bass, Hallucinogen Lead, Dark Forest Pad, Acid Squelch u. a.
  (eigene Presets: `Source/Presets.h`)

## Bauen
Voraussetzungen: CMake >= 3.22, C++17-Compiler.

    cmake -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release

JUCE wird automatisch per FetchContent geladen. Das VST3 liegt danach in
`build/GoaGater_artefacts/Release/VST3/`. Alternativ baut GitHub Actions
(`.github/workflows/build.yml`) die Binaries für Windows/macOS/Linux bei jedem Push.

## Lizenz
JUCE steht unter GPLv3 bzw. kommerzieller Lizenz – Projekt entsprechend lizenzieren.
