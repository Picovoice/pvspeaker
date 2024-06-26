# PvSpeaker

Made in Vancouver, Canada by [Picovoice](https://picovoice.ai)

<!-- markdown-link-check-disable -->
[![Twitter URL](https://img.shields.io/twitter/url?label=%40AiPicovoice&style=social&url=https%3A%2F%2Ftwitter.com%2FAiPicovoice)](https://twitter.com/AiPicovoice)
<!-- markdown-link-check-enable -->
[![YouTube Channel Views](https://img.shields.io/youtube/channel/views/UCAdi9sTCXLosG1XeqDwLx7w?label=YouTube&style=social)](https://www.youtube.com/channel/UCAdi9sTCXLosG1XeqDwLx7w)

PvSpeaker is an easy-to-use, cross-platform audio player designed for real-time speech audio processing. It allows developers to send raw PCM frames to an audio device's output stream.

## Table of Contents
- [PvSpeaker](#pvspeaker)
    - [Table of Contents](#table-of-contents)
    - [Source Code](#source-code)
    - [Demos](#demos)
        - [C](#c-demo)

## Source Code

If you are interested in building PvSpeaker from source or integrating it into an existing C project, the PvSpeaker
source code is located under the [/project](./project) directory.

## Demos

If using SSH, clone the repository with:

```console
git clone --recurse-submodules git@github.com:Picovoice/pvspeaker.git
```

If using HTTPS, clone the repository with:

```console
git clone --recurse-submodules https://github.com/Picovoice/pvspeaker.git
```

### C Demo

Run the following commands to build the demo app:

```console
cd demo/c
cmake -S . -B build -DPV_SPEAKER_PLATFORM={PV_SPEAKER_PLATFORM}
cmake --build build
```

The `{PV_SPEAKER_PLATFORM}` variable will set the compilation flags for the given platform. Exclude this variable
to get a list of possible values.

Get a list of available audio player devices:
```console
./pv_speaker_demo --show_audio_devices
```

Play from a single-channel PCM WAV file with a given audio device index:
```console
./pv_speaker_demo -i test.wav -d 2
```

Hit `Ctrl+C` if you wish to stop audio playback before it completes. If no audio device index (`-d`) is provided, the demo will use the system's default audio player device.

For more information about the C demo, go to [demo/c](demo/c).