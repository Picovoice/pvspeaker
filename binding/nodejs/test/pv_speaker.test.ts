import { PvSpeaker } from "../src";

const DEFAULT_SAMPLE_RATE = 22050;
const DEFAULT_FRAME_LENGTH = 512;
const DEFAULT_BITS_PER_SAMPLE = 16;

describe("Test PvSpeaker", () => {
    test("invalid device index", () => {
        const f = () => {
            new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, DEFAULT_BITS_PER_SAMPLE, -2);
        }

        expect(f).toThrow(Error);
    });

    test("invalid frame length", () => {
        const f = () => {
            new PvSpeaker(DEFAULT_SAMPLE_RATE, 0, DEFAULT_BITS_PER_SAMPLE);
        }

        expect(f).toThrow(Error);
    });

    test("invalid buffered frames count", () => {
        const f = () => {
            new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, DEFAULT_BITS_PER_SAMPLE, 0, 0);
        }

        expect(f).toThrow(Error);
    });

    test("start stop (Int16Array)", async () => {
        const speaker = new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, DEFAULT_BITS_PER_SAMPLE);
        speaker.start();

        let isError = false;
        try {
            const frames = new Int16Array(DEFAULT_FRAME_LENGTH);
            speaker.writeSync(frames);
            await speaker.write(frames);
        } catch (e) {
            isError = true;
        }
        expect(isError).toBeFalsy();

        speaker.release();
    });

    test("start stop (Uint8Array)", async () => {
        const speaker = new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, 8);
        speaker.start();

        let isError = false;
        try {
            const frames = new Uint8Array(DEFAULT_FRAME_LENGTH);
            speaker.writeSync(frames);
            await speaker.write(frames);
        } catch (e) {
            isError = true;
        }
        expect(isError).toBeFalsy();

        speaker.release();
    });

    test("start stop (Int32Array)", async () => {
        const speaker = new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, 32);
        speaker.start();

        let isError = false;
        try {
            const frames = new Int32Array(DEFAULT_FRAME_LENGTH);
            speaker.writeSync(frames);
            await speaker.write(frames);
        } catch (e) {
            isError = true;
        }
        expect(isError).toBeFalsy();

        speaker.release();
    });

    test("set debug logging", () => {
        const speaker = new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, DEFAULT_BITS_PER_SAMPLE);
        speaker.setDebugLogging(true);
        speaker.setDebugLogging(false);
        speaker.release();
    });

    test("is started", () => {
        const speaker = new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, DEFAULT_BITS_PER_SAMPLE);

        speaker.start();
        expect(speaker.isStarted).toBeTruthy();

        speaker.stop();
        expect(speaker.isStarted).toBeFalsy();

        speaker.release();
    });

    test("get selected device", () => {
        const speaker = new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, DEFAULT_BITS_PER_SAMPLE);
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
        const speaker = new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, DEFAULT_BITS_PER_SAMPLE);

        expect(speaker.version).toBeDefined();
        expect(typeof speaker.version).toBe("string");
        expect(speaker.version.length).toBeGreaterThan(0);

        speaker.release();
    });

    test("sample rate", () => {
        const speaker = new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, DEFAULT_BITS_PER_SAMPLE);

        expect(speaker.sampleRate).toBeDefined();
        expect(typeof speaker.sampleRate).toBe("number");
        expect(speaker.sampleRate).toBeGreaterThan(0);

        speaker.release();
    });

    test("frame length", () => {
        const speaker = new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, DEFAULT_BITS_PER_SAMPLE);

        expect(speaker.frameLength).toBeDefined();
        expect(typeof speaker.frameLength).toBe("number");
        expect(speaker.frameLength).toBeGreaterThan(0);

        speaker.release();
    });

    test("bits per sample", () => {
        const speaker = new PvSpeaker(DEFAULT_SAMPLE_RATE, DEFAULT_FRAME_LENGTH, DEFAULT_BITS_PER_SAMPLE);

        expect(speaker.bitsPerSample).toBeDefined();
        expect(typeof speaker.bitsPerSample).toBe("number");
        expect(speaker.bitsPerSample).toBeGreaterThan(0);

        speaker.release();
    });
});