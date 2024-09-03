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

Write PCM data to the speaker:

```python
def get_next_audio_frame():
    pass

speaker.write(get_next_audio_frame())
```

Note: the `write()` method only writes as much PCM data as the internal circular buffer can currently fit, and returns the length of the PCM data that was successfully written.

When all frames have been written, run `flush()` to wait for all buffered pcm data (i.e. previously buffered via `write()`) to be played:

```python
speaker.flush()
```

Note: calling `flush()` with PCM data as an argument will both write that PCM data and wait for all buffered PCM data to finish.

```python
def get_remaining_audio_frames():
    pass

speaker.flush(get_remaining_audio_frames())
```

To stop the audio output device, run `stop()`:

```python
speaker.stop()
```

Note that in order to stop the audio before it finishes playing, `stop` must be run on a separate thread from `flush`.

Once you are done (i.e. no longer need PvSpeaker to write and/or play PCM), free the resources acquired by PvSpeaker by calling `delete`. Be sure to first call `stop` if the audio is still playing. Otherwise, if the audio has already finished playing, you do not have to call `stop` before `delete`:

```python
speaker.delete()
```

## Demos

[pvspeakerdemo](https://pypi.org/project/pvspeakerdemo/) provides command-line utilities for playing audio from a file.