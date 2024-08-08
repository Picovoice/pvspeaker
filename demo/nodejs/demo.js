#! /usr/bin/env node

// Copyright 2024 Picovoice Inc.
//
// You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
// file accompanying this source.
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.
//
"use strict";

const fs = require("fs");
const { program } = require("commander");

const { PvSpeaker } = require("@picovoice/pvspeaker-node");

program
  .option(
    "-s, --show_audio_devices",
    "List of audio devices currently available for use."
  ).option(
    "-d, --audio_device_index <number>",
    "Index of input audio device.",
    Number,
    -1
  ).option(
    "-i, --input_wav_path <string>",
    "Path to PCM WAV file to be played."
  ).option(
    "-b, --buffer_size_secs <number>",
    "Size of internal PCM buffer in seconds.",
    Number,
    20
  ).option(
    "-o, --output_wav_path <string>",
    "Path to the output WAV file where the PCM data passed to PvSpeaker will be written."
  );

if (process.argv.length < 2) {
  program.help();
}
program.parse(process.argv);

function splitArrayBuffer(arrBuf, maxSize) {
  const inputArray = new Uint8Array(arrBuf);
  const result = [];
  for (let i = 0; i < inputArray.length; i += maxSize) {
    let endIndex = Math.min(i + maxSize, inputArray.length);
    const chunk = new Uint8Array(inputArray.slice(i, endIndex));
    result.push(chunk.buffer);
  }

  return result;
}

async function runDemo() {
  let showAudioDevices = program["show_audio_devices"];
  let deviceIndex = program["audio_device_index"];
  let inputWavPath = program["input_wav_path"];
  let bufferSizeSecs = program["buffer_size_secs"];
  let outputPath = program["output_wav_path"];

  if (showAudioDevices) {
    const devices = PvSpeaker.getAvailableDevices();
    for (let i = 0; i < devices.length; i++) {
      console.log(`index: ${i}, device name: ${devices[i]}`)
    }
  } else {
    const wavBuffer = fs.readFileSync(inputWavPath);
    if (wavBuffer.toString('utf8', 0, 4) !== 'RIFF' || wavBuffer.toString('utf8', 8, 12) !== 'WAVE') {
      throw new Error('Invalid WAV file');
    }

    const formatChunkOffset = wavBuffer.indexOf('fmt ', 12);
    if (formatChunkOffset === -1) {
      throw new Error('Invalid WAV file: fmt chunk not found');
    }

    const numChannels = wavBuffer.readUInt16LE(formatChunkOffset + 10);
    const sampleRate = wavBuffer.readUInt32LE(formatChunkOffset + 12);
    const bitsPerSample = wavBuffer.readUInt16LE(formatChunkOffset + 22);

    if (numChannels !== 1) {
      throw new Error('WAV file must have a single channel (MONO)');
    }

    const headerSize = 44;
    const pcmBuffer = wavBuffer.buffer.slice(headerSize);

    try {
      const speaker = new PvSpeaker(sampleRate, bitsPerSample, { bufferSizeSecs, deviceIndex });
      console.log(`Using PvSpeaker version: ${speaker.version}`);
      console.log(`Using device: ${speaker.getSelectedDevice()}`);

      speaker.start();

      if (outputPath) {
        speaker.writeToFile(outputPath);
      }

      console.log("Playing audio...");
      const bytesPerSample = bitsPerSample / 8;
      const pcmChunks = splitArrayBuffer(pcmBuffer, sampleRate * bytesPerSample);
      for (const pcmChunk of pcmChunks) {
        let totalWrittenByteLength = 0;
        while (totalWrittenByteLength < pcmChunk.byteLength) {
          const writtenLength = speaker.write(pcmChunk.slice(totalWrittenByteLength));
          totalWrittenByteLength += (writtenLength * bytesPerSample);
        }
      }

      console.log("Waiting for audio to finish...");
      speaker.flush();

      console.log("Finished playing audio...");
      speaker.stop();
      speaker.release();
    } catch (e) {
      console.log(e.message);
    }
  }
}

(async function () {
  try {
    await runDemo();
  } catch (e) {
    console.error(e.toString());
  }
})();
