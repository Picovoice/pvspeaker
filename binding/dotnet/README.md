# PvSpeaker Binding for .NET

# PvSpeaker

PvSpeaker is an easy-to-use, cross-platform audio player designed for real-time speech audio processing. It allows developers to send raw PCM frames to an audio device's output stream.

## Requirements

- .NET 8.0

## Compatibility

Platform compatible with .NET Framework 4.6.1+:

- Windows (x86_64)

Platforms compatible with .NET Core 2.0+:

- Linux (x86_64)
- macOS (x86_64)
- Windows (x86_64)

Platforms compatible with .NET Core 3.0+:

- Raspberry Pi:
  - 3 (32 and 64 bit)
  - 4 (32 and 64 bit)
  - 5 (32 and 64 bit)

Platform compatible with .NET 6.0+:

- macOS (arm64)

## Installation

You can install the latest version of PvSpeaker by adding the latest [PvSpeaker Nuget package](https://www.nuget.org/packages/PvSpeaker/)
in Visual Studio or using by using the .NET CLI:

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

To have resources freed immediately after use without explicitly calling the `Dispose()` function, wrap `PvSpeaker` in a `using` statement:

```csharp
using (var speaker = new PvSpeaker(sampleRate: 22050, bitsPerSample: 16)) {
    // PvSpeaker usage
}
```

### Selecting an Audio Device

To print a list of available audio devices:
```csharp
string[] devices = PvSpeaker.GetAudioDevices();
```

The index of the device in the returned list can be used in `Create()` to select that device for audio playback:
```csharp
var speaker = new PvSpeaker(
    sampleRate: 22050,
    bitsPerSample: 16,
    deviceIndex: 2);
```

## Demo

The [PvSpeaker .NET demo](https://github.com/Picovoice/pvspeaker/tree/main/demo/dotnet) is a .NET command-line application that demonstrates how to
use PvSpeaker to play audio from a WAV file.