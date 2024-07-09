# PvSpeaker Demo for Node.js

This project contains a Node.js command-line demo for PvSpeaker that demonstrates how to use PvSpeaker to play audio from a WAV file.

## PvSpeaker

PvSpeaker is an easy-to-use, cross-platform audio player designed for real-time speech audio processing. It allows developers to send raw PCM frames to an audio device's output stream.

## Compatibility

- Node.js 14+
- Runs on Linux (x86_64), macOS (x86_64 and arm64), Windows (x86_64), and Raspberry Pi (3, 4, 5).

## Installation:

To install the demos and make them available on the command line, use either of the following `yarn` or `npm` commands:

```console
yarn global add @picovoice/pvspeaker-node-demo
```

(or)

```console
npm install -g @picovoice/pvspeaker-node-demo
```

## Usage

In the following instructions, we will refer to  `{AUDIO_DEVICE_INDEX}` as the index of the audio device to use, and `{INPUT_WAV_PATH}` as the path to the pcm `wav` file that will be played.

`{AUDIO_DEVICE_INDEX}` defaults to -1 and `{INPUT_WAV_PATH}` must not be empty.

To show the available audio devices run:

```console
pvspeaker-node-demo --show_audio_devices
```

To run PvSpeaker run:

```console
pvspeaker-node-demo --audio_device_index {AUDIO_DEVICE_INDEX} --input_wav_path {INPUT_WAV_PATH}
```