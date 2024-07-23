# PvSpeaker Binding for Python

## PvSpeaker

PvSpeaker is an easy-to-use, cross-platform audio player designed for real-time speech audio processing. It allows developers to send raw PCM frames to an audio device's output stream.

## Compatibility

- Python 3.8+
- Runs on Linux (x86_64), macOS (x86_64 and arm64), Windows (x86_64), and Raspberry Pi (3, 4, 5).

## Installation

```shell
pip3 install pvspeaker
```

## Usage

Initialize and start `PvSpeaker`:

```python
from pvspeaker import PvSpeaker

speaker = PvSpeaker(
    sample_rate=22050,
    bits_per_sample=16,
    buffer_size_secs=20,
    device_index=0)

speaker.start()
```

(or)

Use `get_available_devices()` to get a list of available devices and then initialize the instance based on the index of a device:

```python
from pvspeaker import PvSpeaker

devices = PvSpeaker.get_available_devices()

speaker = PvSpeaker(
    sample_rate=22050,
    bits_per_sample=16,
    buffer_size_secs=20,
    device_index=0)

speaker.start()
```

Write frames of audio:

```python
def get_next_audio_frame():
    pass

speaker.write(get_next_audio_frame())
```

When all frames have been written, run `flush()` to wait for all buffered pcm data to be played:

```python
speaker.flush()
```

If you wish to stop playing audio before it completes, run `stop_flush`:

```python
speaker.stop_flush()
```

Note that this function will have to be called in a separate thread.

Once you are done writing pcm, run `stop()` on the instance:

```python
speaker.stop()
```

Once you are done playing audio (i.e. `flush` has completed or `stop_flush` has been called), free the resources acquired by PvSpeaker. You do not have to call `stop()` before `delete()`:

```python
speaker.delete()
```

## Demos

[pvspeakerdemo](https://pypi.org/project/pvspeakerdemo/) provides command-line utilities for playing audio from a file.