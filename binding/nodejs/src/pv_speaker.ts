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
  private readonly _frameLength: number;
  private readonly _version: string;

  /**
   * PvSpeaker constructor.
   *
   * @param sampleRate The sample rate of the audio to be played.
   * @param bitsPerSample The number of bits per sample.
   * @param deviceIndex The audio device index to use to play audio. A value of (-1) will use machine's default audio device.
   * @param frameLength Length of the audio frames to send per write call.
   * @param bufferedFramesCount The number of audio frames buffered internally for writing - i.e. internal circular buffer
   * will be of size `frameLength` * `bufferedFramesCount`. If this value is too low, buffer overflows could occur
   * and audio frames could be dropped. A higher value will increase memory usage.
   */
  constructor(
    sampleRate: number,
    bitsPerSample: number,
    deviceIndex: number = -1,
    frameLength: number = 512,
    bufferedFramesCount = 50,
  ) {
    let pvSpeakerHandleAndStatus;
    try {
      pvSpeakerHandleAndStatus = PvSpeaker._pvSpeaker.init(
        sampleRate, bitsPerSample, deviceIndex, frameLength, bufferedFramesCount);
    } catch (err: any) {
      pvSpeakerStatusToException(err.code, err);
    }
    const status = pvSpeakerHandleAndStatus.status;
    if (status !== PvSpeakerStatus.SUCCESS) {
      throw pvSpeakerStatusToException(status, "PvSpeaker failed to initialize.");
    }
    this._handle = pvSpeakerHandleAndStatus.handle;
    this._sampleRate = sampleRate;
    this._bitsPerSample = bitsPerSample;
    this._frameLength = frameLength;
    this._version = PvSpeaker._pvSpeaker.version();
  }

  /**
   * @returns The sample rate of the audio to be played.
   */
  get sampleRate(): number {
    return this._sampleRate;
  }

  /**
   * @returns The number of bits per sample.
   */
  get bitsPerSample(): number {
    return this._bitsPerSample;
  }

  /**
   * @returns Length of the audio frames to send per write call.
   */
  get frameLength(): number {
    return this._frameLength;
  }

  /**
   * @returns the version of the PvSpeaker
   */
  get version(): string {
    return this._version;
  }

  /**
   * @returns Whether PvSpeaker has started and is available to receive pcm frames or not.
   */
  get isStarted(): boolean {
    return PvSpeaker._pvSpeaker.get_is_started(this._handle);
  }

  /**
   * Starts the audio output device. After starting, pcm frames can be sent to the audio output device via `write`.
   */
  public start(): void {
    const status = PvSpeaker._pvSpeaker.start(this._handle);
    if (status !== PvSpeakerStatus.SUCCESS) {
      throw pvSpeakerStatusToException(status, "PvSpeaker failed to start.");
    }
  }

  /**
   * Stops playing audio.
   */
  public stop(): void {
    const status = PvSpeaker._pvSpeaker.stop(this._handle);
    if (status !== PvSpeakerStatus.SUCCESS) {
      throw pvSpeakerStatusToException(status, "PvSpeaker failed to stop.");
    }
  }

  /**
   * Synchronous call to write a frame of audio data.
   *
   * @returns {Boolean}
   */
  public write(pcm: ArrayBuffer): void {
    let i = 0;
    const frameLength = this._frameLength * this._bitsPerSample / 8;
    while (i < pcm.byteLength) {
      const isLastFrame = i + frameLength >= pcm.byteLength;
      const writeFrameLength = isLastFrame ? pcm.byteLength - i : frameLength;
      const frame = pcm.slice(i, i + writeFrameLength);
      const status = PvSpeaker._pvSpeaker.write(this._handle, this._bitsPerSample, frame);
      if (status !== PvSpeakerStatus.SUCCESS) {
        throw pvSpeakerStatusToException(status, "PvSpeaker failed to write audio data frame.");
      }

      i += frameLength;
    }
  }

  /**
   * Enable or disable debug logging for PvSpeaker. Debug logs will indicate when there are overflows in the internal
   * frame buffer and when an audio source is generating frames of silence.
   *
   * @param isDebugLoggingEnabled Boolean indicating whether the debug logging is enabled or disabled.
   */
  public setDebugLogging(isDebugLoggingEnabled: boolean): void {
    PvSpeaker._pvSpeaker.set_debug_logging(this._handle, isDebugLoggingEnabled);
  }

  /**
   * Returns the name of the selected device used to capture audio.
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
