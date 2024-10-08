# PvSpeaker Binding for Node.js

## PvSpeaker

PvSpeaker is an easy-to-use, cross-platform audio player designed for real-time speech audio processing. It allows developers to send raw PCM frames to an audio device's output stream.

## Compatibility

- Node.js 16+
- Runs on Linux (x86_64), macOS (x86_64 and arm64), Windows (x86_64), and Raspberry Pi (3, 4, 5).

## Installation

```console
yarn add @picovoice/pvspeaker-node
```

## Usage

Initialize and start `PvSpeaker`:

```javascript
const { PvSpeaker } = require("@picovoice/pvspeaker-node");

const sampleRate = 22050;
const bitsPerSample = 16;
const speaker = new PvSpeaker(sampleRate, bitsPerSample);

speaker.start()
```

(or)

Use `getAvailableDevices()` to get a list of available devices and then initialize the instance based on the index of a device:

```javascript

const { PvSpeaker } = require("@picovoice/pvspeaker-node");

const devices = PvSpeaker.getAvailableDevices()

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

## Demos

[@picovoice/pvspeaker-demo](https://www.npmjs.com/package/@picovoice/pvspeaker-demo) provides command-line utilities for playing audio from a file.
