# PvSpeaker Demo for C

This project contains a C command-line demo for PvSpeaker that demonstrates how to use PvSpeaker to play audio from a single-channel PCM WAV file.

## PvSpeaker

PvSpeaker is an easy-to-use, cross-platform audio player designed for real-time speech audio processing. It allows developers to send raw PCM frames to an audio device's output stream.

## Requirements

- CMake 3.4+.
- C99 compatible compiler.
- **Windows**: MinGW.

## Compatibility

- Linux (x86_64)
- macOS (x86_64, arm64)
- Windows (amd64)
- Raspberry Pi (3, 4 and 5)

## Compiling

Run the following commands to build the demo app:

```console
git submodule update --init --recursive
cmake -S . -B build -DPV_SPEAKER_PLATFORM={PV_SPEAKER_PLATFORM}
cmake --build build
```

The `{PV_SPEAKER_PLATFORM}` variable will set the compilation flags for the given platform. Exclude this variable
to get a list of possible values.

## Usage

To see the usage options for the demo:
```console
./pv_speaker_demo
```

Get a list of available audio player devices:
```console
./pv_speaker_demo --show_audio_devices
```

Play from a single-channel PCM WAV file with a given audio device index:
```console
./pv_speaker_demo -i test.wav -d 2
```

Hit `Ctrl+C` if you wish to stop audio playback before it completes. If no audio device index (`-d`) is provided, the demo will use the system's default audio player device.
