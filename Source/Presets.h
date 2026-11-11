#pragma once

// Pattern: 16 Steps, 'x' = Gate offen, '.' = Gate zu (abgesenkt um "depth")
struct GoaPreset
{
    const char* name;
    int   wave;        // 0 Saw, 1 Square
    int   uni;         // 1..7
    float det, sub, cutoff, res, envAmt, fdec;
    float att, dec, sus, rel;
    int   rate;        // 0=1/4 1=1/8 2=1/16 3=1/32
    float depth, smooth;
    const char* pattern;
};

inline const GoaPreset goaPresets[] =
{
 // name                  w  u  det   sub   cutoff res   env   fdec  att    dec   sus   rel   rate dep  smo  pattern
 { "Goa Classic Gate",    0, 5, 0.35f, 0.0f, 2400, 0.35f,0.45f,0.25f,0.005f,0.3f, 0.8f, 0.25f, 2, 1.0f,0.15f,"xx.xxx.xxx.xxx.x" },
 { "Psy Rolling Bass",    0, 1, 0.0f,  0.6f,  500, 0.55f,0.55f,0.12f,0.002f,0.15f,0.7f, 0.08f, 2, 1.0f,0.05f,".xxx.xxx.xxx.xxx" },
 { "Full-On Offbeat",     0, 3, 0.2f,  0.5f,  800, 0.50f,0.60f,0.10f,0.002f,0.2f, 0.7f, 0.10f, 2, 1.0f,0.05f,".x.x.x.x.x.x.x.x" },
 { "Hallucinogen Lead",   0, 7, 0.55f, 0.0f, 3500, 0.60f,0.50f,0.30f,0.005f,0.4f, 0.75f,0.30f, 2, 0.8f,0.20f,"xxxxxxxxxxxxxxxx" },
 { "Dark Forest Pad",     0, 7, 0.70f, 0.2f, 1200, 0.30f,0.30f,0.80f,0.40f, 1.0f, 0.9f, 1.20f, 1, 0.7f,0.60f,"xxxx....xxxx...." },
 { "Pulse 16th",          1, 3, 0.15f, 0.3f, 1800, 0.40f,0.40f,0.20f,0.002f,0.2f, 0.8f, 0.12f, 2, 1.0f,0.10f,"x.x.x.x.x.x.x.x." },
 { "Triplet-ish Stutter", 0, 5, 0.30f, 0.0f, 2800, 0.45f,0.40f,0.20f,0.003f,0.3f, 0.8f, 0.20f, 2, 1.0f,0.08f,"xxx.xxx.xxx.xxx." },
 { "Acid Squelch",        0, 1, 0.0f,  0.4f,  350, 0.85f,0.85f,0.18f,0.002f,0.2f, 0.5f, 0.10f, 2, 0.9f,0.05f,"x.xxx.x.xx.xx.x." },
 { "Trance Supersaw",     0, 7, 0.60f, 0.1f, 4500, 0.25f,0.35f,0.40f,0.010f,0.5f, 0.9f, 0.40f, 2, 1.0f,0.25f,"xx.xxx.xxx.xxx.x" },
 { "Spacey Slow Gate",    0, 7, 0.65f, 0.0f, 2200, 0.35f,0.40f,0.60f,0.05f, 0.8f, 0.9f, 0.80f, 1, 1.0f,0.45f,"xx..xx..xx..xx.." },
 { "Fast 32nd Flutter",   1, 3, 0.20f, 0.2f, 2600, 0.40f,0.30f,0.15f,0.002f,0.2f, 0.9f, 0.10f, 3, 1.0f,0.10f,"x.x.x.x.x.x.x.x." },
 { "Zenon Bass",          0, 3, 0.10f, 0.7f,  420, 0.65f,0.70f,0.09f,0.002f,0.12f,0.7f, 0.06f, 2, 1.0f,0.03f,"x.xxx.xxx.xxx.xx" },
 { "Morning Goa Pad",     0, 7, 0.50f, 0.0f, 3200, 0.20f,0.25f,0.90f,0.30f, 1.0f, 0.9f, 1.50f, 1, 0.5f,0.55f,"xxxxxxxx........" },
 { "Rave Stabs",          1, 5, 0.30f, 0.0f, 3000, 0.50f,0.70f,0.10f,0.002f,0.15f,0.3f, 0.12f, 2, 1.0f,0.05f,"x...x...x..x...x" },
 { "Twilight Arp Gate",   0, 5, 0.35f, 0.2f, 2000, 0.55f,0.55f,0.22f,0.003f,0.3f, 0.7f, 0.18f, 2, 1.0f,0.12f,"xx.x.xx.xx.x.xx." },
 { "Tech Dark Roller",    0, 1, 0.0f,  0.8f,  300, 0.70f,0.65f,0.10f,0.002f,0.10f,0.8f, 0.05f, 2, 1.0f,0.04f,"xxx.xxx.xxx.xxx." },
};

inline constexpr int numGoaPresets = (int) (sizeof (goaPresets) / sizeof (goaPresets[0]));
