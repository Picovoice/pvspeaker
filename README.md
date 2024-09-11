# PvSpeaker

<!-- markdown-link-check-disable -->
[![PyPI](https://img.shields.io/pypi/v/pvspeaker)](https://pypi.org/project/pvspeaker/)
[![npm](https://img.shields.io/npm/v/@picovoice/pvspeaker-node?label=npm%20%5Bnode%5D)](https://www.npmjs.com/package/@picovoice/pvspeaker-node)
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
      - [.NET](#net-demo)
      - [Node.js](#nodejs-demo)
      - [C](#c-demo)
    - [SDKs](#sdks)
      - [Python](#python)
      - [.NET](#net)
      - [Node.js](#nodejs)

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

Replace `{INPUT_WAV_PATH}` with the path to the PCM WAV file you wish to play.

For more information about the Python demos go to [demo/python](demo/python).

### .NET Demo

From [demo/dotnet/PvSpeakerDemo](demo/dotnet/PvSpeakerDemo) run the
following in the terminal to build the demo:

```console
dotnet build
```

Make sure there is a working speaker connected to your device. From [demo/dotnet/PvSpeakerDemo](demo/dotnet/PvSpeakerDemo) run the
following in the terminal:

```console
dotnet run -- --input_wav_path ${INPUT_WAV_FILE}
```

For more information about the .NET demo, go to [demo/dotnet](demo/dotnet).

### Node.js Demo

Install the demo package:

```console
yarn global add @picovoice/pvspeaker-node-demo
```

To show the available audio devices run:

```console
pvspeaker-node-demo --show_audio_devices
```

With a working speaker connected to your device run the following in the terminal:

```console
pvspeaker-node-demo --input_wav_path ${INPUT_WAV_PATH}
```

Replace `{INPUT_WAV_PATH}` with the path to the `wav` file you wish to play.

For more information about Node.js demo, go to [demo/nodejs](demo/nodejs).

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

Hit `Ctrl+C` if you wish to stop playing audio before it completes. If no audio device index (`-d`) is provided, the demo will use the system's default audio player device.

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

Write PCM data to the speaker:

```python
def get_next_audio_frame():
    pass

speaker.write(get_next_audio_frame())
```

Note: the `write()` method only writes as much PCM data as the internal circular buffer can currently fit, and returns the length of the PCM data that was successfully written.

When all frames have been written, run `flush()` to wait for all buffered PCM data (i.e. previously buffered via `write()`) to be played:

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

For more information about the PvSpeaker Python SDK, go to [binding/python](binding/python).

### .NET

Install the .NET SDK using NuGet or the dotnet CLI:

```console
dotnet add package PvSpeaker
```

## Usage

Initialize and start `PvSpeaker`:

```csharp
using Pv;

var speaker = new PvSpeaker(
    sampleRate: 22050,
    bitsPerSample: 16);

speaker.Start();
```

Write PCM data to the speaker:

```csharp
public static byte[] GetNextAudioFrame() { }

int writtenLength = speaker.Write(GetNextAudioFrame());
```

Note: the `Write()` method only writes as much PCM data as the internal circular buffer can currently fit, and returns the number of samples that were successfully written.

When all frames have been written, run `Flush()` to wait for all buffered PCM data (i.e. previously buffered via `Write()`) to be played:

```csharp
int flushedLength = speaker.Flush();
```

Note: calling `Flush()` with PCM data as an argument will both write that PCM data and wait for all buffered PCM data to finish.

```csharp
public static byte[] GetRemainingAudioFrames() { }

int flushedLength = speaker.Flush(GetRemainingAudioFrames());
```

To stop the audio output device, run `Stop()`:

```csharp
speaker.Stop();
```

Once you are done, free the resources acquired by PvSpeaker. You do not have to call `Stop()` before `Dispose()`:

```csharp
speaker.Dispose();
```

For more information about the PvSpeaker .NET SDK, go to [binding/dotnet](binding/dotnet).

### Node.js

Install Node.js binding:

```console
yarn add @picovoice/pvspeaker-node
```

To start playing audio, initialize the instance and run `start()`:

```typescript
const { PvSpeaker } = require("@picovoice/pvspeaker-node");

const sampleRate = 22050;
const bitsPerSample = 16;
const deviceIndex = 0;
const speaker = new PvSpeaker(sampleRate, bitsPerSample, { deviceIndex });

speaker.start()
```

Write PCM data to the speaker:

```typescript
function getNextAudioFrame(): ArrayBuffer {
    //
}

speaker.write(getNextAudioFrame())
```

Note: the `write()` method only writes as much PCM data as the internal circular buffer can currently fit, and returns the length of the PCM data that was successfully written.

When all frames have been written, run `flush()` to wait for all buffered PCM data (i.e. previously buffered via `write()`) to be played:

```typescript
speaker.flush()
```

Note: calling `flush()` with PCM data as an argument will both write that PCM data and wait for all buffered PCM data to finish.

```typescript
function getRemainingAudioFrames(): ArrayBuffer {
    //
}

speaker.flush(getRemainingAudioFrames())
```

To stop the audio output device, run `stop()`:

```typescript
speaker.stop();
```

Once you are done (i.e. no longer need PvSpeaker to write and/or play PCM), free the resources acquired by PvSpeaker by calling `release`. You do not have to call `stop` before `release`:

```typescript
speaker.release();
```

For more information about the PvSpeaker Node.js SDK, go to [binding/nodejs](binding/nodejs).
