#
#    Copyright 2024 Picovoice Inc.
#
#    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
#    file accompanying this source.
#
#    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
#    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
#    specific language governing permissions and limitations under the License.
#

import os
import unittest

from _pvspeaker import *


class PvSpeakerTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        PvSpeaker.set_default_library_path(os.path.join('..', '..'))

    def test_invalid_sample_rate(self):
        with self.assertRaises(ValueError):
            _ = PvSpeaker(0, 16, 20, 0)

    def test_invalid_bits_per_sample(self):
        with self.assertRaises(ValueError):
            _ = PvSpeaker(16000, 0, 20, 0)

    def test_invalid_buffer_size_secs(self):
        with self.assertRaises(ValueError):
            _ = PvSpeaker(16000, 16, 0, 0)

    def test_invalid_device_index(self):
        with self.assertRaises(ValueError):
            _ = PvSpeaker(16000, 16, 20, -2)

    def test_start_stop(self):
        error = False
        try:
            speaker = PvSpeaker(16000, 16, 20)
            speaker.start()
            pcm = [0] * 1000
            speaker.write(pcm)
            speaker.flush(pcm)
            speaker.flush()
            speaker.stop()
            speaker.delete()
        except ValueError or IOError:
            error = True
        self.assertFalse(error)

    def test_write_flow(self):
        sample_rate = 16000
        buffer_size_secs = 1
        circular_buffer_size = sample_rate * buffer_size_secs
        pcm = [0] * (circular_buffer_size + 1)

        speaker = PvSpeaker(sample_rate, 16, buffer_size_secs)
        speaker.start()
        output_path = "tmp.wav"
        speaker.write_to_file(output_path)
        self.assertTrue(os.path.exists(output_path))

        write_count = speaker.write(pcm)
        self.assertEqual(write_count, circular_buffer_size)
        write_count = speaker.flush(pcm)
        self.assertEqual(write_count, len(pcm))
        write_count = speaker.flush()
        self.assertEqual(write_count, 0)

        speaker.stop()
        speaker.delete()
        os.remove(output_path)

    def test_is_started(self):
        speaker = PvSpeaker(16000, 16, 20)
        speaker.start()
        self.assertTrue(speaker.is_started)
        speaker.stop()
        self.assertFalse(speaker.is_started)
        speaker.delete()

    def test_selected_device(self):
        speaker = PvSpeaker(16000, 16, 20)
        device = speaker.selected_device
        self.assertIsNotNone(device)
        self.assertIsInstance(device, str)
        speaker.delete()

    def test_get_available_devices(self):
        speaker = PvSpeaker(16000, 16, 20)
        devices = speaker.get_available_devices()
        self.assertIsNotNone(devices)
        for device in devices:
            self.assertIsNotNone(device)
            self.assertIsInstance(device, str)
        speaker.delete()

    def test_version(self):
        speaker = PvSpeaker(16000, 16, 20)
        version = speaker.version
        self.assertGreater(len(version), 0)
        self.assertIsInstance(version, str)
        speaker.delete()

    def test_sample_rate(self):
        speaker = PvSpeaker(16000, 16, 20)
        sample_rate = speaker.sample_rate
        self.assertEqual(sample_rate, 16000)
        self.assertIsInstance(sample_rate, int)
        speaker.delete()

    def test_bits_per_sample(self):
        speaker = PvSpeaker(16000, 16, 20)
        bits_per_sample = speaker.bits_per_sample
        self.assertEqual(bits_per_sample, 16)
        self.assertIsInstance(bits_per_sample, int)
        speaker.delete()

    def test_buffer_size_secs(self):
        speaker = PvSpeaker(16000, 16, 20)
        buffer_size_secs = speaker.buffer_size_secs
        self.assertEqual(buffer_size_secs, 20)
        self.assertIsInstance(buffer_size_secs, int)
        speaker.delete()


if __name__ == '__main__':
    unittest.main()
