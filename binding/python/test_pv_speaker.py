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

import os.path
import unittest

from _pvspeaker import *


class PvSpeakerTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        PvSpeaker.set_default_library_path(os.path.join('..', '..'))

    def test_invalid_device_index(self):
        with self.assertRaises(ValueError):
            _ = PvSpeaker(16000, 16, -2)

    def test_invalid_frame_length(self):
        with self.assertRaises(ValueError):
            _ = PvSpeaker(16000, 16, 0, 0)

    def test_invalid_buffered_frame_count(self):
        with self.assertRaises(ValueError):
            _ = PvSpeaker(16000, 16, 0, 512, 0)

    def test_set_frame_length(self):
        speaker = PvSpeaker(16000, 16, 0, 256)
        frame_length = speaker.frame_length
        self.assertEqual(frame_length, 256)
        self.assertIsInstance(frame_length, int)
        speaker.delete()

    def test_start_stop(self):
        speaker = PvSpeaker(16000, 16, 0)
        speaker.start()
        frame = [0] * (512 * 2)
        is_success = speaker.write(frame)
        self.assertTrue(is_success)
        speaker.stop()
        speaker.delete()

    def test_set_debug_logging(self):
        speaker = PvSpeaker(16000, 16, 0)
        speaker.set_debug_logging(True)
        speaker.set_debug_logging(False)
        self.assertIsNotNone(speaker)
        speaker.delete()

    def test_is_playing(self):
        speaker = PvSpeaker(16000, 16, 0)
        speaker.start()
        self.assertTrue(speaker.is_playing)
        speaker.stop()
        self.assertFalse(speaker.is_playing)
        speaker.delete()

    def test_selected_device(self):
        speaker = PvSpeaker(16000, 16, 0)
        device = speaker.selected_device
        self.assertIsNotNone(device)
        self.assertIsInstance(device, str)
        speaker.delete()

    def test_get_available_devices(self):
        speaker = PvSpeaker(16000, 16, 0)
        devices = speaker.get_available_devices()
        self.assertIsNotNone(devices)
        for device in devices:
            self.assertIsNotNone(device)
            self.assertIsInstance(device, str)
        speaker.delete()

    def test_version(self):
        speaker = PvSpeaker(16000, 16, 0)
        version = speaker.version
        self.assertGreater(len(version), 0)
        self.assertIsInstance(version, str)
        speaker.delete()

    def test_sample_rate(self):
        speaker = PvSpeaker(16000, 16, 0)
        sample_rate = speaker.sample_rate
        self.assertEqual(sample_rate, 16000)
        self.assertIsInstance(sample_rate, int)
        speaker.delete()

    def test_frame_length(self):
        speaker = PvSpeaker(16000, 16, 0)
        frame_length = speaker.frame_length
        self.assertEqual(frame_length, 512)
        self.assertIsInstance(frame_length, int)
        speaker.delete()

    def test_bits_per_sample(self):
        speaker = PvSpeaker(16000, 16, 0)
        bits_per_sample = speaker.bits_per_sample
        self.assertEqual(bits_per_sample, 16)
        self.assertIsInstance(bits_per_sample, int)
        speaker.delete()

if __name__ == '__main__':
    unittest.main()