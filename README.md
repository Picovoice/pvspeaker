# PvSpeaker

<!-- markdown-link-check-disable -->
[![PyPI](https://img.shields.io/pypi/v/pvspeaker)](https://pypi.org/project/pvspeaker/)
<!-- markdown-link-check-enable -->

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
      - [Python](#python-demo)
      - [C](#c-demo)
    - [SDKs](#sdks)
      - [Python](#python) 

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

### Python Demo

Install the demo package:

```console
pip3 install pvspeakerdemo
```

To show the available audio devices run:

```console
pv_speaker_demo --show_audio_devices
```

With a working speaker connected to your device run the following in the terminal:

```console
pv_speaker_demo --input_wav_path {INPUT_WAV_PATH}
```

Replace `{INPUT_WAV_PATH}` with the path to the pcm `wav` file you wish to play.

For more information about the Python demos go to [demo/python](demo/python).

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

## SDKs

### Python

To start playing audio, initialize an instance and run `start()`:

```python
from pvspeaker import PvSpeaker

speaker = PvSpeaker(
    sample_rate=22050,
    bits_per_sample=16,
    device_index=0)
speaker.start()
```

Write frames of audio:

```python
frame = [0] * 512
speaker.write(frame)
```

When all frames have been written, run `stop()` on the instance:

```python
speaker.stop()
```

Once you are done, free the resources acquired by PvSpeaker. You do not have to call `stop()` before `delete()`:

```python
speaker.delete()
```

For more information about the PvSpeaker Python SDK, go to [binding/python](binding/python).