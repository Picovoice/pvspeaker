//
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

import { execSync } from "child_process";
import * as os from "os";
import * as path from "path";

import PvSpeakerStatus from "./pv_speaker_status_t";
import pvSpeakerStatusToException from "./errors";

/**
 * PvSpeaker class for playing audio.
 */
class PvSpeaker {
  // eslint-disable-next-line
  private static _pvSpeaker = require(PvSpeaker._getLibraryPath());

  private readonly _handle: number;
  private readonly _sampleRate: number;
  private readonly _bitsPerSample: number;
  private readonly _bufferSizeSecs: number;
  private readonly _version: string;

  /**
   * PvSpeaker constructor.
   *
   * @param sampleRate The sample rate of the audio to be played.
   * @param bitsPerSample The number of bits per sample.
   * @param options Optional configuration arguments.
   * @param options.bufferSizeSecs The size in seconds of the internal buffer used to buffer PCM data
   * - i.e. internal circular buffer will be of size `sampleRate` * `bufferSizeSecs`.
   * @param options.deviceIndex The audio device index to use to play audio. A value of (-1) will use the machine's default audio device.
   */
  constructor(
    sampleRate: number,
    bitsPerSample: number,
    options: {
      bufferSizeSecs?: number;
      deviceIndex?: number;
    } = {}
  ) {
    let pvSpeakerHandleAndStatus;
    const { bufferSizeSecs = 20, deviceIndex = -1 } = options;
    try {
      pvSpeakerHandleAndStatus = PvSpeaker._pvSpeaker.init(sampleRate, bitsPerSample, bufferSizeSecs, deviceIndex);
    } catch (err: any) {
      throw pvSpeakerStatusToException(err.code, err);
    }
    const status = pvSpeakerHandleAndStatus.status;
    if (status !== PvSpeakerStatus.SUCCESS) {
      throw pvSpeakerStatusToException(status, "Failed to initialize PvSpeaker.");
    }
    this._handle = pvSpeakerHandleAndStatus.handle;
    this._sampleRate = sampleRate;
    this._bitsPerSample = bitsPerSample;
    this._bufferSizeSecs = bufferSizeSecs;
    this._version = PvSpeaker._pvSpeaker.version();
  }

  /**
   * @returns {number} The sample rate matching the value passed to the constructor.
   */
  get sampleRate(): number {
    return this._sampleRate;
  }

  /**
   * @returns {number} The bits per sample matching the value passed to the constructor.
   */
  get bitsPerSample(): number {
    return this._bitsPerSample;
  }

  /**
   * @returns {number} The buffer size in seconds matching the value passed to the constructor.
   */
  get bufferSizeSecs(): number {
    return this._bufferSizeSecs;
  }

  /**
   * @returns {number} The current version of PvSpeaker library.
   */
  get version(): string {
    return this._version;
  }

  /**
   * @returns {boolean} Whether the speaker has started and is available to receive pcm frames or not.
   */
  get isStarted(): boolean {
    return PvSpeaker._pvSpeaker.get_is_started(this._handle);
  }

  /**
   * Starts the audio output device.
   */
  public start(): void {
    const status = PvSpeaker._pvSpeaker.start(this._handle);
    if (status !== PvSpeakerStatus.SUCCESS) {
      throw pvSpeakerStatusToException(status, "Failed to start device.");
    }
  }

  /**
   * Stops the audio output device.
   */
  public stop(): void {
    const status = PvSpeaker._pvSpeaker.stop(this._handle);
    if (status !== PvSpeakerStatus.SUCCESS) {
      throw pvSpeakerStatusToException(status, "Failed to stop device.");
    }
  }

  /**
   * Synchronous call to write PCM data to the internal circular buffer for audio playback.
   * Only writes as much PCM data as the internal circular buffer can currently fit, and
   * returns the length of the PCM data that was successfully written.
   *
   * @param {ArrayBuffer} pcm PCM data to be played.
   * @returns {number} Length of the PCM data that was successfully written.
   */
  public write(pcm: ArrayBuffer): number {
    const result = PvSpeaker._pvSpeaker.write(this._handle, this._bitsPerSample, pcm);
    if (result.status !== PvSpeakerStatus.SUCCESS) {
      throw pvSpeakerStatusToException(result.status, "Failed to write to device.");
    }

    return result.written_length;
  }

  /**
   * Synchronous call to write PCM data to the internal circular buffer for audio playback.
   * This call blocks the thread until all PCM data has been successfully written and played.
   *
   * @param {ArrayBuffer} pcm PCM data to be played.
   * @returns {number} The length of the PCM data that was successfully written.
   */
  public flush(pcm: ArrayBuffer = new ArrayBuffer(0)): number {
    const result = PvSpeaker._pvSpeaker.flush(this._handle, this._bitsPerSample, pcm);
    if (result.status !== PvSpeakerStatus.SUCCESS) {
      throw pvSpeakerStatusToException(result.status, "Failed to flush PCM data.");
    }

    return result.written_length;
  }

  /**
   * Writes PCM data passed to PvSpeaker to a specified WAV file.
   *
   * @param {string} outputPath Path to the output WAV file where the PCM data will be written.
   */
  public writeToFile(outputPath: string): void {
    const status = PvSpeaker._pvSpeaker.write_to_file(this._handle, outputPath);
    if (status !== PvSpeakerStatus.SUCCESS) {
      throw pvSpeakerStatusToException(status, "Failed to open FILE object. PCM data will not be written.");
    }
  }

  /**
   * Gets the audio device that the given PvSpeaker instance is using.
   *
   * @returns {string} Name of the selected audio device.
   */
  public getSelectedDevice(): string {
    const device = PvSpeaker._pvSpeaker.get_selected_device(this._handle);
    if ((device === undefined) || (device === null)) {
      throw new Error("Failed to get selected device.");
    }
    return device;
  }

  /**
   * Destructor. Releases resources acquired by PvSpeaker.
   */
  public release(): void {
    PvSpeaker._pvSpeaker.delete(this._handle);
  }

  /**
   * Helper function to get the list of available audio devices.
   *
   * @returns {Array<string>} An array of the available device names.
   */
  public static getAvailableDevices(): string[] {
    const devices = PvSpeaker._pvSpeaker.get_available_devices();
    if ((devices === undefined) || (devices === null)) {
      throw new Error("Failed to get audio devices.");
    }
    return devices;
  }

  private static _getLibraryPath(): string {
    let scriptPath;
    if (os.platform() === "win32") {
      scriptPath = path.resolve(__dirname, "..", "scripts", "platform.bat");
    } else {
      scriptPath = path.resolve(__dirname, "..", "scripts", "platform.sh");
    }

    const output = execSync(`"${scriptPath}"`).toString();
    const [osName, cpu] = output.split(" ");

    return path.resolve(__dirname, "..", "lib", osName, cpu, "pv_speaker.node");
  }
}

export default PvSpeaker;
