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

let isInterrupted = false;

program
    .option(
        "-i, --audio_device_index <number>",
        "index of audio device to use to play audio",
        Number,
        -1
    ).option(
    "-d, --show_audio_devices",
    "show the list of available devices"
).option(
    "-p, --input_wav_path <string>",
    "path to wav file to play audio from"
);

if (process.argv.length < 2) {
    program.help();
}
program.parse(process.argv);

async function runDemo() {
    let audioDeviceIndex = program["audio_device_index"];
    let showAudioDevices = program["show_audio_devices"];
    let inputWavPath = program["input_wav_path"];

    if (showAudioDevices) {
        const devices = PvSpeaker.getAvailableDevices();
        for (let i = 0; i < devices.length; i++) {
            console.log(`index: ${i}, device name: ${devices[i]}`)
        }
    } else {
        const buffer = fs.readFileSync(inputWavPath);

        if (buffer.toString('ascii', 0, 4) !== 'RIFF' || buffer.toString('ascii', 8, 12) !== 'WAVE') {
            throw new Error('Invalid WAV file');
        }

        const formatChunkOffset = buffer.indexOf('fmt ', 12);
        if (formatChunkOffset === -1) {
            throw new Error('Invalid WAV file: fmt chunk not found');
        }

        const numChannels = buffer.readUInt16LE(formatChunkOffset + 10);
        const sampleRate = buffer.readUInt32LE(formatChunkOffset + 12);
        const bitsPerSample = buffer.readUInt16LE(formatChunkOffset + 22);

        if (numChannels !== 1) {
            throw new Error('Not MONO');
        }

        const dataChunkOffset = buffer.indexOf('data', formatChunkOffset + 24);
        if (dataChunkOffset === -1) {
            throw new Error('Invalid WAV file: data chunk not found');
        }

        const dataChunkSize = buffer.readUInt32LE(dataChunkOffset + 4);
        const pcmDataStart = dataChunkOffset + 8;
        const pcmDataEnd = pcmDataStart + dataChunkSize;
        const pcmDataBuffer = buffer.slice(pcmDataStart, pcmDataEnd);

        let pcm;
        switch (bitsPerSample) {
            case 8:
                pcm = new Uint8Array(pcmDataBuffer);
                break;
            case 16:
                pcm = new Int16Array(pcmDataBuffer.buffer, pcmDataBuffer.byteOffset, pcmDataBuffer.byteLength / 2);
                break;
            case 32:
                pcm = new Int32Array(pcmDataBuffer.buffer, pcmDataBuffer.byteOffset, pcmDataBuffer.byteLength / 4);
                break;
            default:
                throw new Error(`Unsupported bits per sample: ${bitsPerSample}`);
        }

        console.log("Playing audio...");

        const frameLength = 512;
        // try to break
        const speaker = new PvSpeaker(sampleRate, frameLength, bitsPerSample, audioDeviceIndex);
        console.log(`Using PvSpeaker version: ${speaker.version}`);

        speaker.start();
        console.log(`Using device: ${speaker.getSelectedDevice()}`);

        speaker.writeSync(pcm);

        speaker.stop();

        console.log("Stopping...");
        speaker.release();

        console.log("Finished playing audio...");
    }
}

// setup interrupt
process.on("SIGINT", function () {
    isInterrupted = true;
});

(async function () {
    try {
        await runDemo();
    } catch (e) {
        console.error(e.toString());
    }
})();