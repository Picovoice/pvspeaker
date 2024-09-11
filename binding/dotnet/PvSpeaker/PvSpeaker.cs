/*
    Copyright 2024 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

using System;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;

namespace Pv
{
    /// <summary>
    /// Status codes returned by the PvSpeaker Library.
    /// </summary>
    public enum PvSpeakerStatus
    {
        SUCCESS = 0,
        OUT_OF_MEMORY = 1,
        INVALID_ARGUMENT = 2,
        INVALID_STATE = 3,
        BACKEND_ERROR = 4,
        DEVICE_ALREADY_INITIALIZED = 5,
        DEVICE_NOT_INITIALIZED = 6,
        IO_ERROR = 7,
        RUNTIME_ERROR = 8
    }

    /// <summary>
    /// PvSpeaker is a cross-platform audio playback library for .NET that is designed for real-time speech audio processing.
    /// </summary>
    public class PvSpeaker : IDisposable
    {
        private const string LIBRARY = "libpv_speaker";
        private IntPtr _libraryPointer = IntPtr.Zero;

        static PvSpeaker()
        {

#if NETCOREAPP3_0_OR_GREATER

            NativeLibrary.SetDllImportResolver(typeof(PvSpeaker).Assembly, ImportResolver);

#endif

        }

#if NETCOREAPP3_0_OR_GREATER

        private static IntPtr ImportResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
        {

#pragma warning disable IDE0058
#pragma warning disable IDE0059

            IntPtr libHandle = IntPtr.Zero;
            NativeLibrary.TryLoad(GetLibraryPath(), out libHandle);
            return libHandle;
        }

#pragma warning restore IDE0059
#pragma warning restore IDE0058

#endif

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern PvSpeakerStatus pv_speaker_init(int sampleRate, int bitsPerSample, int bufferSizeSecs, int deviceIndex, out IntPtr handle);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern void pv_speaker_delete(IntPtr handle);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern PvSpeakerStatus pv_speaker_start(IntPtr handle);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern PvSpeakerStatus pv_speaker_stop(IntPtr handle);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern PvSpeakerStatus pv_speaker_write(IntPtr handle, IntPtr pcm, int pcmLength, out int writtenLength);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern PvSpeakerStatus pv_speaker_flush(IntPtr handle, IntPtr pcm, int pcmLength, out int writtenLength);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern PvSpeakerStatus pv_speaker_write_to_file(IntPtr handle, IntPtr outputPath);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern char pv_speaker_get_is_started(IntPtr handle);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr pv_speaker_get_selected_device(IntPtr handle);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern PvSpeakerStatus pv_speaker_get_available_devices(out int deviceListLength, out IntPtr deviceList);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern void pv_speaker_free_available_devices(int deviceListLength, IntPtr deviceList);

        [DllImport(LIBRARY, CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr pv_speaker_version();

        /// <summary>
        /// Factory method for creating instances of PvSpeaker.
        /// </summary>
        /// <param name="sampleRate">
        /// The sample rate of the audio to be played.
        /// </param>
        /// <param name="bitsPerSample">
        /// The number of bits per sample of the audio to be played.
        /// </param>
        /// <param name="bufferSizeSecs">
        /// The size in seconds of the internal buffer used to buffer PCM data
        /// - i.e. internal circular buffer will be of size `sampleRate` * `bufferSizeSecs`.
        /// </param>
        /// <param name="deviceIndex">
        /// The index of the audio device to play audio from. A value of (-1) will use the default audio device.
        /// </param>
        /// <returns>An instance of PvSpeaker.</returns>
        public static PvSpeaker Create(int sampleRate, int bitsPerSample, int bufferSizeSecs = 20, int deviceIndex = -1)
        {
            return new PvSpeaker(sampleRate, bitsPerSample, bufferSizeSecs, deviceIndex);
        }

        /// <summary>
        /// Constructor for PvSpeaker.
        /// </summary>
        /// <param name="sampleRate">
        /// The sample rate of the audio to be played.
        /// </param>
        /// <param name="bitsPerSample">
        /// The number of bits per sample of the audio to be played.
        /// </param>
        /// <param name="bufferSizeSecs">
        /// The size in seconds of the internal buffer used to buffer PCM data
        /// - i.e. internal circular buffer will be of size `sampleRate` * `bufferSizeSecs`.
        /// </param>
        /// <param name="deviceIndex">
        /// The index of the audio device to play audio from. A value of (-1) will use the default audio device.
        /// </param>
        private PvSpeaker(int sampleRate, int bitsPerSample, int bufferSizeSecs, int deviceIndex)
        {
            if (sampleRate <= 0)
            {
                throw new PvSpeakerInvalidArgumentException($"Sample rate of {sampleRate} is invalid - must be greater than 0.");
            }

            if (bitsPerSample != 8 && bitsPerSample != 16 && bitsPerSample != 24 && bitsPerSample != 32)
            {
                throw new PvSpeakerInvalidArgumentException($"Bits per sample of {bitsPerSample} is invalid - must be 8, 16, 24, or 32.");
            }

            if (bufferSizeSecs <= 0)
            {
                throw new PvSpeakerInvalidArgumentException($"Buffer size (in seconds) of {bufferSizeSecs} is invalid - must be greater than 0.");
            }

            if (deviceIndex < -1)
            {
                throw new PvSpeakerInvalidArgumentException($"Device index of {deviceIndex} is invalid - must be greater than -1.");
            }

            PvSpeakerStatus status = pv_speaker_init(sampleRate, bitsPerSample, bufferSizeSecs, deviceIndex, out _libraryPointer);
            if (status != PvSpeakerStatus.SUCCESS)
            {
                throw PvSpeakerStatusToException(status, "Failed to initialize PvSpeaker.");
            }

            SampleRate = sampleRate;
            BitsPerSample = bitsPerSample;
            BufferSizeSecs = bufferSizeSecs;
            SelectedDevice = Marshal.PtrToStringAnsi(pv_speaker_get_selected_device(_libraryPointer));
            Version = Marshal.PtrToStringAnsi(pv_speaker_version());
        }

        /// <summary>
        /// Starts the audio output device. Should be called before making any calls to `Write()`, `Flush()`, or `Stop()`.
        /// </summary>
        public void Start()
        {
            PvSpeakerStatus status = pv_speaker_start(_libraryPointer);
            if (status != PvSpeakerStatus.SUCCESS)
            {
                throw PvSpeakerStatusToException(status, "Failed to start PvSpeaker.");
            }
        }

        /// <summary>
        /// Stops the audio output device. Should only be called after a successful call to `Start()`.
        /// </summary>
        public void Stop()
        {
            PvSpeakerStatus status = pv_speaker_stop(_libraryPointer);
            if (status != PvSpeakerStatus.SUCCESS)
            {
                throw PvSpeakerStatusToException(status, "Failed to stop PvSpeaker.");
            }
        }

        /// <summary>
        /// Synchronous call to write PCM data to the internal circular buffer for audio playback.
        /// Only writes as much PCM data as the internal circular buffer can currently fit, and returns
        /// the number of samples that were successfully written. Call between `Start()` and `Stop()`.
        /// </summary>
        /// <param name="pcm">PCM data to be played.</param>
        /// <returns>Number of samples that were successfully written.</returns>
        public int Write(byte[] pcm)
        {
            // Create a pointer to the byte array to pass to `pv_speaker_write` .
            GCHandle pinnedArray = GCHandle.Alloc(pcm, GCHandleType.Pinned);
            IntPtr pcmPtr = pinnedArray.AddrOfPinnedObject();

            PvSpeakerStatus status = pv_speaker_write(_libraryPointer, pcmPtr, pcm.Length / (BitsPerSample / 8), out int writtenLength);
            pinnedArray.Free();
            if (status != PvSpeakerStatus.SUCCESS)
            {
                throw PvSpeakerStatusToException(status, "Failed to write to PvSpeaker.");
            }
            return writtenLength;
        }

        /// <summary>
        /// Synchronous call to write PCM data to the internal circular buffer for audio playback.
        /// This call blocks the thread until all PCM data has been successfully written and played.
        /// Call between `Start()` and `Stop()`.
        /// </summary>
        /// <param name="pcm">PCM data to be played.</param>
        /// <returns>Number of samples that were successfully written.</returns>
        public int Flush(byte[] pcm = null)
        {
            pcm = pcm ?? Array.Empty<byte>();

            // Create a pointer to the byte array to pass to `pv_speaker_flush`.
            GCHandle pinnedArray = GCHandle.Alloc(pcm, GCHandleType.Pinned);
            IntPtr pcmPtr = pinnedArray.AddrOfPinnedObject();

            PvSpeakerStatus status = pv_speaker_flush(_libraryPointer, pcmPtr, pcm.Length / (BitsPerSample / 8), out int writtenLength);
            pinnedArray.Free();
            if (status != PvSpeakerStatus.SUCCESS)
            {
                throw PvSpeakerStatusToException(status, "Failed to flush PCM data from PvSpeaker.");
            }
            return writtenLength;
        }

        /// <summary>
        /// Writes PCM data passed to PvSpeaker to a specified WAV file.
        /// </summary>
        /// <param name="outputPath">Path to the output WAV file where the PCM data passed to PvSpeaker will be written.</param>
        public void WriteToFile(string outputPath)
        {
            if (String.IsNullOrEmpty(outputPath))
            {
                throw new PvSpeakerInvalidArgumentException("Output file path was empty.");
            }

            byte[] utf8Bytes = Encoding.UTF8.GetBytes(outputPath + '\0');
            IntPtr outputPathPtr = Marshal.AllocHGlobal(utf8Bytes.Length);
            Marshal.Copy(utf8Bytes, 0, outputPathPtr, utf8Bytes.Length);

            PvSpeakerStatus status = pv_speaker_write_to_file(_libraryPointer, outputPathPtr);
            if (status != PvSpeakerStatus.SUCCESS)
            {
                throw PvSpeakerStatusToException(status, "Failed to write to output file.");
            }
        }

        /// <summary>
        /// Gets whether the speaker has started and is available to receive PCM data or not.
        /// </summary>
        public bool IsStarted
        {
            get
            {
                return pv_speaker_get_is_started(_libraryPointer) != 0;
            }
        }

        /// <summary>
        /// Gets the sample rate matching the value passed to the constructor.
        /// </summary>
        public int SampleRate
        {
            get; private set;
        }

        /// <summary>
        /// Gets the bits per sample matching the value passed to the constructor.
        /// </summary>
        public int BitsPerSample
        {
            get; private set;
        }

        /// <summary>
        /// Gets the buffer size in seconds matching the value passed to the constructor.
        /// </summary>
        public int BufferSizeSecs
        {
            get; private set;
        }

        /// <summary>
        /// Gets the current selected audio device.
        /// </summary>
        public string SelectedDevice
        {
            get; private set;
        }

        /// <summary>
        /// Gets the current version of the library.
        /// </summary>
        public string Version
        {
            get; private set;
        }

        /// <summary>
        /// Gets a list of the available audio output devices on the current system.
        /// </summary>
        /// <returns>An array of strings containing the names of the audio devices.</returns>
        public static string[] GetAvailableDevices()
        {
            PvSpeakerStatus status = pv_speaker_get_available_devices(out int deviceListLength, out IntPtr deviceList);
            if (status != PvSpeakerStatus.SUCCESS)
            {
                throw PvSpeakerStatusToException(status, "Failed to get available output devices.");
            }

            int elementSize = Marshal.SizeOf(typeof(IntPtr));
            string[] deviceNames = new string[deviceListLength];
            for (int i = 0; i < deviceListLength; i++)
            {
                deviceNames[i] = Marshal.PtrToStringAnsi(Marshal.ReadIntPtr(deviceList, i * elementSize));
            }

            pv_speaker_free_available_devices(deviceListLength, deviceList);

            return deviceNames;
        }

        /// <summary>
        /// Converts status codes to PvSpeakerExceptions.
        /// </summary>
        /// <param name="status">Status code.</param>
        /// <returns>PvSpeakerExceptions</returns>
        private static PvSpeakerException PvSpeakerStatusToException(PvSpeakerStatus status, string message = "")
        {
            switch (status)
            {
                case PvSpeakerStatus.OUT_OF_MEMORY:
                    return new PvSpeakerMemoryException(message);
                case PvSpeakerStatus.INVALID_ARGUMENT:
                    return new PvSpeakerInvalidArgumentException(message);
                case PvSpeakerStatus.INVALID_STATE:
                    return new PvSpeakerInvalidStateException(message ?? "PvSpeaker failed with invalid state.");
                case PvSpeakerStatus.BACKEND_ERROR:
                    return new PvSpeakerBackendException(message ?? "PvSpeaker audio backend error.");
                case PvSpeakerStatus.DEVICE_ALREADY_INITIALIZED:
                    return new PvSpeakerDeviceAlreadyInitializedException(message ?? "PvSpeaker audio device already initialized.");
                case PvSpeakerStatus.DEVICE_NOT_INITIALIZED:
                    return new PvSpeakerDeviceNotInitializedException(message ?? "PvSpeaker audio device not initialized.");
                case PvSpeakerStatus.IO_ERROR:
                    return new PvSpeakerIOException(message);
                case PvSpeakerStatus.RUNTIME_ERROR:
                    return new PvSpeakerRuntimeException(message ?? "PvSpeaker runtime error.");
                default:
                    return new PvSpeakerException("Unknown status returned from PvSpeaker.");
            }
        }

        /// <summary>
        /// Helper function to get the library path of pv_speaker.
        /// </summary>
        /// <returns>A string representing the absolute path of the library.</returns>
        private static string GetLibraryPath()
        {
            string scriptPath = Path.Combine(
                AppContext.BaseDirectory,
                RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? "scripts/platform.bat" : "scripts/platform.sh"
            );

            var process = new Process();
            var processStartInfo = new ProcessStartInfo()
            {
                FileName = scriptPath,
                UseShellExecute = false,
                RedirectStandardOutput = true
            };
            process.StartInfo = processStartInfo;
            process.Start();
            process.WaitForExit();

            if (process.ExitCode != 0)
            {
                throw new SystemException("System is not supported.");
            }

            string[] output = process.StandardOutput.ReadToEnd().Split(' ');
            string osName = output[0];
            string cpu = output[1];

            string libName;

            if (osName == "windows")
            {
                libName = $"{LIBRARY}.dll";
            }
            else if (osName == "mac")
            {
                libName = $"{LIBRARY}.dylib";
            }
            else
            {
                libName = $"{LIBRARY}.so";
            }

            return Path.Combine(AppContext.BaseDirectory, $"lib/{osName}/{cpu}/{libName}");
        }

        /// <summary>
        /// Frees memory used by PvSpeaker instance.
        /// </summary>
        public void Dispose()
        {
            if (_libraryPointer != IntPtr.Zero)
            {
                pv_speaker_delete(_libraryPointer);
                _libraryPointer = IntPtr.Zero;

                GC.SuppressFinalize(this);
            }
        }

        ~PvSpeaker()
        {
            Dispose();
        }
    }
}