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

import os
import platform
import subprocess
from ctypes import *
from enum import Enum
from typing import *

CALLBACK = CFUNCTYPE(None, POINTER(c_int16))


def default_library_path(relative: str = ''):
    """A helper function to get the library path."""

    if platform.system() == "Windows":
        script_path = os.path.join(os.path.dirname(__file__), relative, "resources", "scripts", "platform.bat")
    else:
        script_path = os.path.join(os.path.dirname(__file__), relative, "resources", "scripts", "platform.sh")

    command = subprocess.run(script_path, stdout=subprocess.PIPE)

    if command.returncode != 0:
        raise RuntimeError("Current system is not supported.")
    os_name, cpu = str(command.stdout.decode("utf-8")).split(" ")

    if os_name == "windows":
        extension = "dll"
    elif os_name == "mac":
        extension = "dylib"
    else:
        extension = "so"

    return os.path.join(os.path.dirname(__file__), relative, "lib", os_name, cpu, "libpv_speaker.%s" % extension)


class PvSpeaker(object):
    """
    A cross-platform Python SDK for PvSpeaker to play audio. It lists the available output devices.
    Also given the audio device index, sample_rate, frame_length, and bits_per_sample, plays the
    frame of audio to the device speakers.
    """

    class PvSpeakerStatuses(Enum):
        SUCCESS = 0
        OUT_OF_MEMORY = 1
        INVALID_ARGUMENT = 2
        INVALID_STATE = 3
        BACKEND_ERROR = 4
        DEVICE_ALREADY_INITIALIZED = 5
        DEVICE_NOT_INITIALIZED = 6
        IO_ERROR = 7
        RUNTIME_ERROR = 8

    _PVSPEAKER_STATUS_TO_EXCEPTION = {
        PvSpeakerStatuses.OUT_OF_MEMORY: MemoryError,
        PvSpeakerStatuses.INVALID_ARGUMENT: ValueError,
        PvSpeakerStatuses.INVALID_STATE: ValueError,
        PvSpeakerStatuses.BACKEND_ERROR: SystemError,
        PvSpeakerStatuses.DEVICE_ALREADY_INITIALIZED: ValueError,
        PvSpeakerStatuses.DEVICE_NOT_INITIALIZED: ValueError,
        PvSpeakerStatuses.IO_ERROR: IOError,
        PvSpeakerStatuses.RUNTIME_ERROR: RuntimeError
    }

    class CPvSpeaker(Structure):
        pass

    _library = None
    _relative_library_path = ''

    def __init__(
            self,
            sample_rate: int,
            frame_length: int,
            bits_per_sample: int,
            device_index: int = -1,
            buffered_frames_count: int = 50):
        """
        Constructor

        :param sample_rate: The sample rate of the audio to be played.
        :param frame_length: The maximum length of audio frame that will be passed to each write call.
        :param bits_per_sample: The number of bits per sample.
        :param device_index: The index of the audio device to use. A value of (-1) will resort to default device.
        :param buffered_frames_count: The number of audio frames buffered internally for writing - i.e. internal
        circular buffer will be of size `frame_length` * `buffered_frames_count`. If this value is too low,
        buffer overflows could occur audio frames could be dropped. A higher value will increase memory usage.
        """

        library = self._get_library()

        init_func = library.pv_speaker_init
        init_func.argtypes = [
            c_int32,
            c_int32,
            c_int32,
            c_int32,
            c_int32,
            POINTER(POINTER(self.CPvSpeaker))
        ]
        init_func.restype = self.PvSpeakerStatuses

        self._handle = POINTER(self.CPvSpeaker)()
        self._sample_rate = sample_rate
        self._frame_length = frame_length
        self._bits_per_sample = bits_per_sample

        status = init_func(
            sample_rate, frame_length, bits_per_sample, device_index, buffered_frames_count, byref(self._handle))
        if status is not self.PvSpeakerStatuses.SUCCESS:
            raise self._PVSPEAKER_STATUS_TO_EXCEPTION[status]("Failed to initialize PvSpeaker.")

        self._delete_func = library.pv_speaker_delete
        self._delete_func.argtypes = [POINTER(self.CPvSpeaker)]
        self._delete_func.restype = None

        self._start_func = library.pv_speaker_start
        self._start_func.argtypes = [POINTER(self.CPvSpeaker)]
        self._start_func.restype = self.PvSpeakerStatuses

        self._stop_func = library.pv_speaker_stop
        self._stop_func.argtypes = [POINTER(self.CPvSpeaker)]
        self._stop_func.restype = self.PvSpeakerStatuses

        self._set_debug_logging_func = library.pv_speaker_set_debug_logging
        self._set_debug_logging_func.argtypes = [POINTER(self.CPvSpeaker), c_bool]
        self._set_debug_logging_func.restype = None

        self._write_func = library.pv_speaker_write
        self._write_func.argtypes = [POINTER(self.CPvSpeaker), c_int32, c_char_p]
        self._write_func.restype = self.PvSpeakerStatuses

        self._get_is_playing_func = library.pv_speaker_get_is_playing
        self._get_is_playing_func.argtypes = [POINTER(self.CPvSpeaker)]
        self._get_is_playing_func.restype = c_bool

        self._get_selected_device_func = library.pv_speaker_get_selected_device
        self._get_selected_device_func.argtypes = [POINTER(self.CPvSpeaker)]
        self._get_selected_device_func.restype = c_char_p

        self._version_func = library.pv_speaker_version
        self._version_func.argtypes = None
        self._version_func.restype = c_char_p

    def delete(self) -> None:
        """Releases any resources used by PvSpeaker."""

        self._delete_func(self._handle)

    def start(self) -> None:
        """Starts buffering audio frames."""

        status = self._start_func(self._handle)
        if status is not self.PvSpeakerStatuses.SUCCESS:
            raise self._PVSPEAKER_STATUS_TO_EXCEPTION[status]("Failed to start device.")

    def stop(self) -> None:
        """Stops playing audio."""

        status = self._stop_func(self._handle)
        if status is not self.PvSpeakerStatuses.SUCCESS:
            raise self._PVSPEAKER_STATUS_TO_EXCEPTION[status]("Failed to stop device.")

    def write(self, frame) -> True:
        """Synchronous call to write a frame of audio."""

        status = self._write_func(self._handle, c_int32(len(frame)), c_char_p(id(frame)))
        if status is not self.PvSpeakerStatuses.SUCCESS:
            if len(frame) > self.frame_length:
                raise self._PVSPEAKER_STATUS_TO_EXCEPTION[status]("Failed to write to device. Frame length must not be "
                                                                  "greater than `frame_length` given to `__init__()`")
            else:
                raise self._PVSPEAKER_STATUS_TO_EXCEPTION[status]("Failed to write to device.")
        else:
            return True

    def set_debug_logging(self, is_debug_logging_enabled: bool) -> None:
        """
        Enable or disable debug logging for PvSpeaker. Debug logs will indicate when there are overflows
        in the internal frame buffer.

        :param is_debug_logging_enabled: Boolean indicating whether the debug logging is enabled or disabled.
        """

        self._set_debug_logging_func(self._handle, is_debug_logging_enabled)

    @property
    def is_playing(self) -> bool:
        """Gets whether the speaker is currently playing audio or not."""

        return bool(self._get_is_playing_func(self._handle))

    @property
    def selected_device(self) -> str:
        """Gets the audio device that the given `PvSpeaker` instance is using."""

        device_name = self._get_selected_device_func(self._handle)
        return device_name.decode('utf-8')

    @property
    def version(self) -> str:
        """Gets the current version of PvSpeaker library."""

        version = self._version_func()
        return version.decode('utf-8')

    @property
    def sample_rate(self) -> int:
        """Gets the sample rate matching the value given to `__init__()`."""

        return self._sample_rate

    @property
    def frame_length(self) -> int:
        """Gets the frame length matching the value given to `__init__()`."""

        return self._frame_length

    @property
    def bits_per_sample(self) -> int:
        """Gets the bits per sample matching the value given to `__init__()`."""

        return self._bits_per_sample

    @staticmethod
    def get_available_devices() -> List[str]:
        """Gets the list of available audio devices that can be used for playing.

        :return: A list of strings, indicating the names of audio devices.
        """

        get_available_devices_func = PvSpeaker._get_library().pv_speaker_get_available_devices
        get_available_devices_func.argstype = [POINTER(c_int32), POINTER(POINTER(c_char_p))]
        get_available_devices_func.restype = PvSpeaker.PvSpeakerStatuses

        free_available_devices_func = PvSpeaker._get_library().pv_speaker_free_available_devices
        free_available_devices_func.argstype = [c_int32, POINTER(c_char_p)]
        free_available_devices_func.restype = None

        count = c_int32()
        devices = POINTER(c_char_p)()

        status = get_available_devices_func(byref(count), byref(devices))
        if status is not PvSpeaker.PvSpeakerStatuses.SUCCESS:
            raise PvSpeaker._PVSPEAKER_STATUS_TO_EXCEPTION[status]("Failed to get device list")

        device_list = list()
        for i in range(count.value):
            device_list.append(devices[i].decode('utf-8'))

        free_available_devices_func(count, devices)

        return device_list

    @classmethod
    def set_default_library_path(cls, relative: str):
        cls._relative_library_path = default_library_path(relative)

    @classmethod
    def _get_library(cls):
        if len(cls._relative_library_path) == 0:
            cls._relative_library_path = default_library_path()
        if cls._library is None:
            cls._library = cdll.LoadLibrary(cls._relative_library_path)
        return cls._library

__all__ = [
    'PvSpeaker',
]