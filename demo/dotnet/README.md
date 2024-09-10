# PvSpeaker Demo for .NET

This project contains a .NET command-line demo for PvSpeaker that demonstrates how to use PvSpeaker to play audio from a WAV file.

## PvSpeaker

PvSpeaker is an easy-to-use, cross-platform audio player designed for real-time speech audio processing. It allows developers to send raw PCM frames to an audio device's output stream.

## Requirements

- .NET 8.0

## Compatibility

- Linux (x86_64)
- macOS (x86_64, arm64)
- Windows (x86_64)
- Raspberry Pi:
    - 3 (32 and 64 bit)
    - 4 (32 and 64 bit)
    - 5 (32 and 64 bit)

## Installation

This demo uses [Microsoft's .NET SDK](https://dotnet.microsoft.com/download).

Build with the dotnet CLI:

```console
dotnet build
```

## Usage

To build the demo:

```console
cd demo/dotnet/PvSpeakerDemo
dotnet build
```

To show the available audio devices run:

```console
dotnet run -- --show_audio_devices
```

To run the demo, give it a file from which to play audio:

```console
dotnet run -- --input_wav_path ${INPUT_WAV_FILE}
```

You can also select the audio device index to use for playback (use `--show_audio_devices` to see options):

```console
dotnet run -- --input_wav_path ${INPUT_WAV_FILE} --audio_device_index ${DEVICE_INDEX}
```
