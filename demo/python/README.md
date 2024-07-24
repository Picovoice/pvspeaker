# PvSpeaker Demo for Python

This project contains a Python command-line demo for PvSpeaker that demonstrates how to use PvSpeaker to play audio from a WAV file.

## PvSpeaker

PvSpeaker is an easy-to-use, cross-platform audio player designed for real-time speech audio processing. It allows developers to send raw PCM frames to an audio device's output stream.

## Compatibility

- Python 3.8+
- Runs on Linux (x86_64), macOS (x86_64 and arm64), Windows (x86_64), and Raspberry Pi (3, 4, 5).

## Installation

```console
pip3 install pvspeakerdemo
```

## Usage

In the following instructions, we will refer to  `{AUDIO_DEVICE_INDEX}` as the index of the audio device to use, and `{INPUT_WAV_PATH}` as the path to the pcm `wav` file that will be played.

`{AUDIO_DEVICE_INDEX}` defaults to -1 and `{INPUT_WAV_PATH}` must not be empty.

To show the available audio devices run:

```console
pv_speaker_demo --show_audio_devices
```

To run PvSpeaker run:

```console
pv_speaker_demo --audio_device_index {AUDIO_DEVICE_INDEX} --input_wav_path {INPUT_WAV_PATH}
```

Hit `Ctrl+C` if you wish to stop playing audio before it completes. If no audio device index (`-d`) is provided, the demo will use the system's default audio player device.