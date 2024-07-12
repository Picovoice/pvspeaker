#
# Copyright 2024 Picovoice Inc.
#
# You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
# file accompanying this source.
#
# Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
# an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
# specific language governing permissions and limitations under the License.
#


import argparse
import wave
import array

from pvspeaker import PvSpeaker


def main():
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "--show_audio_devices",
        help="List of audio devices currently available for use.",
        action="store_true")

    parser.add_argument(
        "--audio_device_index",
        help="Index of input audio device.",
        type=int,
        default=-1)

    parser.add_argument(
        "--input_wav_path",
        help="Path to PCM WAV file to be played.",
        default=None)

    parser.add_argument(
        "--buffer_size_secs",
        help="Size of internal pcm buffer in seconds.",
        type=int,
        default=20)

    args = parser.parse_args()

    if args.show_audio_devices:
        devices = PvSpeaker.get_available_devices()
        for i in range(len(devices)):
            print("index: %d, device name: %s" % (i, devices[i]))
    else:
        device_index = args.audio_device_index
        input_path = args.input_wav_path
        buffer_size_secs = args.buffer_size_secs

        wavfile = None
        speaker = None

        try:
            if input_path is not None:
                wavfile = wave.open(input_path, "rb")

                sample_rate = wavfile.getframerate()
                bits_per_sample = wavfile.getsampwidth() * 8
                num_channels = wavfile.getnchannels()
                num_samples = wavfile.getnframes()

                if bits_per_sample != 8 and bits_per_sample != 16 and bits_per_sample != 24 and bits_per_sample != 32:
                    print(f"Unsupported bits per sample: {bits_per_sample}")
                    wavfile.close()
                    exit()

                if num_channels != 1:
                    print("WAV file must have a single channel (MONO)")
                    wavfile.close()
                    exit()

                speaker = PvSpeaker(
                    sample_rate=sample_rate,
                    bits_per_sample=bits_per_sample,
                    buffer_size_secs=buffer_size_secs,
                    device_index=device_index)
                print("pvspeaker version: %s" % speaker.version)
                print("Using device: %s" % speaker.selected_device)

                wav_bytes = wavfile.readframes(num_samples)

                pcm = None
                if bits_per_sample == 8:
                    pcm = list(array.array('B', wav_bytes))
                elif bits_per_sample == 16:
                    pcm = list(array.array('h', wav_bytes))
                elif bits_per_sample == 24:
                    pcm = []
                    for i in range(0, len(wav_bytes), 3):
                        sample = int.from_bytes(wav_bytes[i:i + 3], byteorder='little', signed=True)
                        pcm.append(sample)
                elif bits_per_sample == 32:
                    pcm = list(array.array('i', wav_bytes))

                speaker.start()

                print("Playing audio...")
                speaker.write(pcm)
                speaker.flush()
                speaker.stop()

                print("Finished playing audio...")
                wavfile.close()

        except KeyboardInterrupt:
            print("Stopping...")
        finally:
            if speaker is not None:
                speaker.delete()
            if wavfile is not None:
                wavfile.close()


if __name__ == "__main__":
    main()
