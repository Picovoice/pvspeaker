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
    "show the list of available devices"
  ).option(
    "-d, --audio_device_index <number>",
    "index of audio device to use to play audio",
    Number,
    -1
  ).option(
    "-i, --input_wav_path <string>",
    "path to PCM WAV file to be played"
  ).option(
    "-b, --buffer_size_secs <number>",
    "size of internal PCM buffer in seconds",
    Number,
    20
  );

if (process.argv.length < 2) {
  program.help();
}
program.parse(process.argv);

function splitList(inputArrayBuffer, maxSublistLength) {
  const inputArray = new Uint8Array(inputArrayBuffer);
  const result = [];
  for (let i = 0; i < inputArray.length; i += maxSublistLength) {
    let endIndex = Math.min(i + maxSublistLength, inputArray.length);
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

    const dataChunkOffset = wavBuffer.indexOf('data', formatChunkOffset + 24);
    if (dataChunkOffset === -1) {
      throw new Error('Invalid WAV file: data chunk not found');
    }

    const headerSize = 44;
    const pcmBuffer = wavBuffer.buffer.slice(headerSize);

    let speaker = null;
    try {
      speaker = new PvSpeaker(sampleRate, bitsPerSample, { bufferSizeSecs, deviceIndex });
      console.log(`Using PvSpeaker version: ${speaker.version}`);
      console.log(`Using device: ${speaker.getSelectedDevice()}`);

      speaker.start();

      console.log("Playing audio...");
      const bytesPerSample = bitsPerSample / 8;
      const pcmList = splitList(pcmBuffer, sampleRate * bytesPerSample);

      pcmList.forEach(pcmSublist => {
        let sublistLength = pcmSublist.byteLength / bytesPerSample;
        let totalWrittenLength = 0;
        while (totalWrittenLength < sublistLength) {
          let remainingBuffer = pcmSublist.slice(totalWrittenLength);
          let writtenLength = speaker.write(remainingBuffer);
          totalWrittenLength += writtenLength;
        }
      });

      console.log("Waiting for audio to finish...");
      speaker.flush();

      console.log("Finished playing audio...");
      speaker.stop();
    } catch (e) {
      console.log(e.message);
    } finally {
      speaker?.release();
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
