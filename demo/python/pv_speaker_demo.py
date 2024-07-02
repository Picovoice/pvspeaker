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
        help="Path to file from which to read raw audio.",
        default=None)

    args = parser.parse_args()

    if args.show_audio_devices:
        devices = PvSpeaker.get_available_devices()
        for i in range(len(devices)):
            print("index: %d, device name: %s" % (i, devices[i]))
    else:
        device_index = args.audio_device_index
        input_path = args.input_wav_path

        recorder = PvSpeaker(sample_rate=16000, frame_length=512, bits_per_sample=16, device_index=device_index)
        print("pvspeaker version: %s" % recorder.version)


if __name__ == "__main__":
    main()