/*
    Copyright 2024 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;

using Pv;

namespace PvSpeakerDemo
{
    class Demo
    {
        /// <summary>
        /// Splits a byte array into a list of byte arrays.
        /// Meant to simulate getting contiguous PCM chunks from a streaming source to pass to PvSpeaker. 
        /// </summary>
        /// <param name="array">Complete byte array to be split up</param>
        /// <param name="maxSize">Maximum size of each byte array in the resulting list</param>
        public static List<byte[]> SplitArray(byte[] array, int maxSize)
        {
            var result = new List<byte[]>();
            for (int i = 0; i < array.Length; i += maxSize)
            {
                int length = Math.Min(maxSize, array.Length - i);
                byte[] pcmChunk = new byte[length];
                Array.Copy(array, i, pcmChunk, 0, pcmChunk.Length);
                result.Add(pcmChunk);
            }

            return result;
        }

        public class WavFileInfo
        {
            public int SampleRate { get; set; }
            public int BitsPerSample { get; set; }
            public byte[] AudioData { get; set; }
        }

        /// <summary>
        /// Reads a WAV file and returns appropriate metadata.
        /// </summary>
        /// <param name="filePath">Absolute path to the target file.</param>
        /// <returns>The target WAV file's sample rate, bits per sample, and audio data.</returns>
        public static WavFileInfo GetWavFileInfo(string filePath)
        {
            if (string.IsNullOrEmpty(filePath))
            {
                throw new ArgumentException("File path cannot be null or empty.");
            }

            if (!File.Exists(filePath))
            {
                throw new FileNotFoundException("The specified file was not found.");
            }

            using (var fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read))
            using (var binaryReader = new BinaryReader(fileStream))
            {
                string chunkId = new string(binaryReader.ReadChars(4));
                if (chunkId != "RIFF")
                    throw new InvalidDataException("Not a valid WAV file.");

                binaryReader.BaseStream.Seek(4, SeekOrigin.Current);
                string format = new string(binaryReader.ReadChars(4));
                if (format != "WAVE")
                    throw new InvalidDataException("Not a valid WAV file.");

                string formatChunkId = new string(binaryReader.ReadChars(4));
                if (formatChunkId != "fmt ")
                    throw new InvalidDataException("Invalid WAV file format.");

                int formatChunkSize = binaryReader.ReadInt32();
                int audioFormat = binaryReader.ReadInt16();
                int channels = binaryReader.ReadInt16();
                int sampleRate = binaryReader.ReadInt32();
                int byteRate = binaryReader.ReadInt32();
                int blockAlign = binaryReader.ReadInt16();
                int bitsPerSample = binaryReader.ReadInt16();

                if (formatChunkSize > 16)
                    binaryReader.BaseStream.Seek(formatChunkSize - 16, SeekOrigin.Current);

                if (channels != 1)
                    throw new InvalidDataException("WAV file must have a single channel (MONO)");

                string dataChunkId = new string(binaryReader.ReadChars(4));
                if (dataChunkId != "data")
                    throw new InvalidDataException("Invalid WAV file data chunk.");

                int dataChunkSize = binaryReader.ReadInt32();
                byte[] audioData = binaryReader.ReadBytes(dataChunkSize);

                return new WavFileInfo
                {
                    SampleRate = sampleRate,
                    BitsPerSample = bitsPerSample,
                    AudioData = audioData
                };
            }
        }

        /// <summary>
        /// Lists available audio input devices.
        /// </summary>
        public static void ShowAudioDevices()
        {
            string[] devices = PvSpeaker.GetAvailableDevices();
            for (int i = 0; i < devices.Length; i++)
            {
                Console.WriteLine($"index: {i}, device name: {devices[i]}");
            }
        }

        private static readonly string HELP_STR = "Available options: \n" +
            " --show_audio_devices: List of audio devices currently available for use.\n" +
            " --audio_device_index: Index of output audio device.\n" +
            " --input_wav_path: Path to PCM WAV file to be played.\n" +
            " --buffer_size_secs: Size of internal PCM buffer in seconds.\n" +
            " --output_wav_path: Path to the output WAV file where the PCM data passed to PvSpeaker will be written.\n";

        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine(HELP_STR);
                Console.Read();
                return;
            }

            bool showAudioDevices = false;
            int audioDeviceIndex = -1;
            string inputWavPath = null;
            int bufferSizeSecs = 20;
            string outputWavPath = null;
            bool showHelp = false;

            int argIndex = 0;
            while (argIndex < args.Length)
            {
                if (args[argIndex] == "--show_audio_devices")
                {
                    showAudioDevices = true;
                    argIndex++;
                }
                else if (args[argIndex] == "--audio_device_index")
                {
                    if (++argIndex < args.Length && int.TryParse(args[argIndex], out int deviceIndex))
                    {
                        audioDeviceIndex = deviceIndex;
                        argIndex++;
                    }
                }
                else if (args[argIndex] == "--input_wav_path")
                {
                    if (++argIndex < args.Length)
                    {
                        inputWavPath = args[argIndex++];
                    }
                }
                else if (args[argIndex] == "--buffer_size_secs")
                {
                    if (++argIndex < args.Length && int.TryParse(args[argIndex], out int bufferSize))
                    {
                        bufferSizeSecs = bufferSize;
                        argIndex++;
                    }
                }
                else if (args[argIndex] == "--output_wav_path")
                {
                    if (++argIndex < args.Length)
                    {
                        outputWavPath = args[argIndex++];
                    }
                }
                else if (args[argIndex] == "-h" || args[argIndex] == "--help")
                {
                    showHelp = true;
                    argIndex++;
                }
                else
                {
                    argIndex++;
                }
            }

            // print help text and exit
            if (showHelp)
            {
                Console.WriteLine(HELP_STR);
                Console.Read();
                return;
            }

            if (showAudioDevices)
            {
                ShowAudioDevices();
                return;
            }

            try
            {
                WavFileInfo wavInfo = GetWavFileInfo(inputWavPath);

                using (PvSpeaker speaker = PvSpeaker.Create(
                        sampleRate: wavInfo.SampleRate,
                        bitsPerSample: wavInfo.BitsPerSample,
                        bufferSizeSecs: bufferSizeSecs,
                        deviceIndex: audioDeviceIndex))
                {
                    Console.CancelKeyPress += delegate (object sender, ConsoleCancelEventArgs e)
                    {
                        e.Cancel = true;
                        speaker.Stop();
                        Console.WriteLine("Stopping...");
                    };

                    speaker.Start();

                    if (outputWavPath != null)
                    {
                        speaker.WriteToFile(outputWavPath);
                    }

                    Console.WriteLine($"Using PvSpeaker version: {speaker.Version}");

                    int bytesPerSample = wavInfo.BitsPerSample / 8;
                    List<byte[]> pcmList = SplitArray(wavInfo.AudioData, wavInfo.SampleRate * bytesPerSample);

                    Console.WriteLine($"Playing {inputWavPath}...");
                    foreach (byte[] pcmSublist in pcmList)
                    {
                        int totalWrittenLength = 0;
                        while (totalWrittenLength < pcmSublist.Length)
                        {
                            int writtenLength = speaker.Write(pcmSublist.Skip(totalWrittenLength).ToArray());
                            totalWrittenLength += (writtenLength * bytesPerSample);
                        }
                    }

                    speaker.Flush();
                }
            }
            catch (Exception e)
            {
                Console.WriteLine($"Exception: {e.Message}");
            }
        }
    }
}