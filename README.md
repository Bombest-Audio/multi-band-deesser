# Multi-Band De-Esser

A JUCE audio plugin combining multi-band de-essing with per-band attenuation blend and an SSL Fusion–style high-frequency limiter.

## Features

- **4 fixed frequency bands**: 2–4 kHz, 4–7 kHz, 7–10 kHz, 10 kHz+
- **Single threshold** (Waves DE-esser style): one global threshold for all bands
- **Per-band attenuation blend**: 0–100% amount per band so you can target which ranges get reduced
- **HF limiter**: soft limiter on content above ~6 kHz (SSL Fusion style)

## Build

```bash
cd multi-band-deesser
git submodule update --init   # if JUCE submodule not initialized
mkdir build && cd build
cmake ..
cmake --build .
```

Plugin formats: VST3, AU, Standalone.

## Parameters

| Parameter | Range | Description |
|-----------|-------|-------------|
| Threshold | -24 to 0 dB | Global de-esser threshold |
| 2-4k | 0–100% | Attenuation blend for lower sibilance |
| 4-7k | 0–100% | Attenuation blend for main sibilance |
| 7-10k | 0–100% | Attenuation blend for harsh "sh" |
| 10k+ | 0–100% | Attenuation blend for air/harshness |
| HF Limit | On/Off | Enable high-frequency limiter |
