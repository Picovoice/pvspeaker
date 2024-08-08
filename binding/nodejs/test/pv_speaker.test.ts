import { PvSpeaker } from "../src";

const fs = require('fs');
const path = require('path');

const SAMPLE_RATE = 22050;
const BITS_PER_SAMPLE = 16;
const BUFFER_SIZE_SECS = 10;

describe("Test PvSpeaker", () => {
  test("invalid sample rate", () => {
    expect(() => {
      new PvSpeaker(0, BITS_PER_SAMPLE);
    }).toThrow(Error);
  });

  test("invalid bits per sample", () => {
    expect(() => {
      new PvSpeaker(SAMPLE_RATE, 0);
    }).toThrow(Error);
  });

  test("invalid buffer size secs", () => {
    expect(() => {
      new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE, { bufferSizeSecs: 0 });
    }).toThrow(Error);
  });

  test("invalid device index", () => {
    expect(() => {
      new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE, { deviceIndex: -2 });
    }).toThrow(Error);
  });

  test("start stop", async () => {
    expect(() => {
      const pcm = new ArrayBuffer(SAMPLE_RATE);
      const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);

      speaker.start();
      speaker.write(pcm);
      speaker.flush(pcm);
      speaker.flush();
      speaker.stop();
      speaker.release();
    }).not.toThrow(Error);
  });

  test("write flow", async () => {
    const bufferSizeSecs = 1;
    const circularBufferSize = SAMPLE_RATE * bufferSizeSecs;
    const bytesPerSample = (BITS_PER_SAMPLE / 8);
    const pcm = new ArrayBuffer(circularBufferSize * bytesPerSample + bytesPerSample);
    const pcmLength = pcm.byteLength / bytesPerSample;

    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE, { bufferSizeSecs });
    speaker.start();

    const outputPath = path.join(__dirname, "tmp.wav");
    speaker.writeToFile(outputPath);
    const fileExists = fs.existsSync(outputPath);
    expect(fileExists).toBe(true);

    let writeCount = speaker.write(pcm);
    expect(writeCount).toBe(circularBufferSize);
    writeCount = speaker.flush(pcm);
    expect(writeCount).toBe(pcmLength);
    writeCount = speaker.flush();
    expect(writeCount).toBe(0);

    speaker.stop();
    speaker.release();
    fs.unlinkSync(outputPath);
  });

  test("is started", () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);

    speaker.start();
    expect(speaker.isStarted).toBeTruthy();
    speaker.stop();
    expect(speaker.isStarted).toBeFalsy();
    speaker.release();
  });

  test("get selected device", () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);
    const device = speaker.getSelectedDevice();

    expect(device).toBeDefined();
    expect(typeof device).toBe("string");

    speaker.release();
  });

  test("get available devices", () => {
    const devices = PvSpeaker.getAvailableDevices();

    for (const device of devices) {
      expect(device).toBeDefined();
      expect(typeof device).toBe("string");
    }
  });

  test("version", () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);

    expect(speaker.version).toBeDefined();
    expect(typeof speaker.version).toBe("string");
    expect(speaker.version.length).toBeGreaterThan(0);

    speaker.release();
  });

  test("sample rate", () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);

    expect(speaker.sampleRate).toBeDefined();
    expect(typeof speaker.sampleRate).toBe("number");
    expect(speaker.sampleRate).toBe(SAMPLE_RATE);

    speaker.release();
  });

  test("bits per sample", () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);

    expect(speaker.bitsPerSample).toBeDefined();
    expect(typeof speaker.bitsPerSample).toBe("number");
    expect(speaker.bitsPerSample).toBe(BITS_PER_SAMPLE);

    speaker.release();
  });

  test("buffer size secs", () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE, { bufferSizeSecs: BUFFER_SIZE_SECS });

    expect(speaker.bufferSizeSecs).toBeDefined();
    expect(typeof speaker.bufferSizeSecs).toBe("number");
    expect(speaker.bufferSizeSecs).toBe(BUFFER_SIZE_SECS);

    speaker.release();
  });
});
