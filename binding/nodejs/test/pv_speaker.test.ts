import { PvSpeaker } from "../src";

const SAMPLE_RATE = 22050;
const BITS_PER_SAMPLE = 16;
const FRAME_LENGTH = 512;

describe("Test PvSpeaker", () => {
  test("invalid device index", () => {
    const f = () => {
      new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE, -2);
    };

    expect(f).toThrow(Error);
  });

  test("invalid frame length", () => {
    const f = () => {
      new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE, 0, 0);
    };

    expect(f).toThrow(Error);
  });

  test("invalid buffered frames count", () => {
    const f = () => {
      new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE, 0, FRAME_LENGTH, 0);
    };

    expect(f).toThrow(Error);
  });

  test("start stop", async () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);
    speaker.start();

    const f = async () => {
      const frames = new Int16Array(FRAME_LENGTH * 2).buffer;
      speaker.write(frames);
      speaker.release();
    }

    expect(f).not.toThrow(Error);
  });

  test("set debug logging", () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);
    speaker.setDebugLogging(true);
    speaker.setDebugLogging(false);
    speaker.release();
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

  test("bits per sample", () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);

    expect(speaker.bitsPerSample).toBeDefined();
    expect(typeof speaker.bitsPerSample).toBe("number");
    expect(speaker.bitsPerSample).toBeGreaterThan(0);

    speaker.release();
  });

  test("sample rate", () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);

    expect(speaker.sampleRate).toBeDefined();
    expect(typeof speaker.sampleRate).toBe("number");
    expect(speaker.sampleRate).toBeGreaterThan(0);

    speaker.release();
  });

  test("frame length", () => {
    const speaker = new PvSpeaker(SAMPLE_RATE, BITS_PER_SAMPLE);

    expect(speaker.frameLength).toBeDefined();
    expect(typeof speaker.frameLength).toBe("number");
    expect(speaker.frameLength).toBeGreaterThan(0);

    speaker.release();
  });
});
