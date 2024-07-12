/*
    Copyright 2024 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#ifndef PV_SPEAKER_H
#define PV_SPEAKER_H

#include <stdbool.h>
#include <stdint.h>

#if __PV_PLATFORM_WINDOWS__

#define PV_API __attribute__ ((dllexport))

#else

#define PV_API __attribute__((visibility ("default")))

#endif

/**
* Struct representing the PvSpeaker object.
*/
typedef struct pv_speaker pv_speaker_t;

/**
* Status codes.
*/
typedef enum {
    PV_SPEAKER_STATUS_SUCCESS = 0,
    PV_SPEAKER_STATUS_OUT_OF_MEMORY,
    PV_SPEAKER_STATUS_INVALID_ARGUMENT,
    PV_SPEAKER_STATUS_INVALID_STATE,
    PV_SPEAKER_STATUS_BACKEND_ERROR,
    PV_SPEAKER_STATUS_DEVICE_ALREADY_INITIALIZED,
    PV_SPEAKER_STATUS_DEVICE_NOT_INITIALIZED,
    PV_SPEAKER_STATUS_IO_ERROR,
    PV_SPEAKER_STATUS_RUNTIME_ERROR
} pv_speaker_status_t;

/**
* Creates a PvSpeaker instance. When finished with the instance, resources should be released
* using the `pv_speaker_delete() function.
*
* @param sample_rate The sample rate of the audio to be played.
* @param bits_per_sample The number of bits per sample.
* @param buffer_size_secs The size in seconds of the internal buffer used to buffer pcm data
* - i.e. internal circular buffer will be of size `sample_rate` * `buffer_size_secs`.
* @param device_index The index of the audio device to use. A value of (-1) will resort to default device.
* @param[out] object PvSpeaker object to be initialized.
* @return Status Code. PV_SPEAKER_STATUS_INVALID_ARGUMENT, PV_SPEAKER_STATUS_BACKEND_ERROR,
* PV_SPEAKER_STATUS_DEVICE_INITIALIZED or PV_SPEAKER_STATUS_OUT_OF_MEMORY on failure.
*/
PV_API pv_speaker_status_t pv_speaker_init(
        int32_t sample_rate,
        int16_t bits_per_sample,
        int32_t buffer_size_secs,
        int32_t device_index,
        pv_speaker_t **object);

/**
* Releases resources acquired by PvSpeaker.
*
* @param object PvSpeaker object.
*/
PV_API void pv_speaker_delete(pv_speaker_t *object);

/**
* Starts the audio output device. After starting, pcm data can be sent to the audio output device via `pv_speaker_write`.
*
* @param object PvSpeaker object.
* @returnStatus Status Code. Returns PV_SPEAKER_STATUS_INVALID_ARGUMENT, PV_SPEAKER_STATUS_DEVICE_NOT_INITIALIZED
* or PV_SPEAKER_STATUS_INVALID_STATE on failure.
*/
PV_API pv_speaker_status_t pv_speaker_start(pv_speaker_t *object);

/**
* Synchronous call to write pcm data.
*
* @param object PvSpeaker object.
* @param pcm Pointer to the pcm data that will be written.
* @param pcm_length Size of the pcm data that is passed in.
* @return Status Code. Returns PV_SPEAKER_STATUS_INVALID_ARGUMENT, PV_SPEAKER_INVALID_STATE or PV_SPEAKER_IO_ERROR on failure.
*/
PV_API pv_speaker_status_t pv_speaker_write(pv_speaker_t *object, int8_t *pcm, int32_t pcm_length);

/**
* Waits for buffered pcm data to be played.
*
* @param object PvSpeaker object.
* @return Status Code. Returns PV_SPEAKER_STATUS_INVALID_ARGUMENT or PV_SPEAKER_STATUS_INVALID_STATE on failure.
*/
PV_API pv_speaker_status_t pv_speaker_flush(pv_speaker_t *object);

/**
* Stops the device.
*
* @param object PvSpeaker object.
* @return Status Code. Returns PV_SPEAKER_STATUS_INVALID_ARGUMENT or PV_SPEAKER_STATUS_INVALID_STATE on failure.
*/
PV_API pv_speaker_status_t pv_speaker_stop(pv_speaker_t *object);

/**
* Gets whether the given `pv_speaker_t` instance has started and available to receive pcm frames or not.
*
* @param object PvSpeaker object.
* @returns A boolean indicating whether PvSpeaker has started and available to receive pcm frames or not.
*/
PV_API bool pv_speaker_get_is_started(pv_speaker_t *object);

/**
* Gets the audio device that the given `pv_speaker_t` instance is using.
*
* @param object PvSpeaker object.
* @return A string containing the name of the current audio output device.
*/
PV_API const char *pv_speaker_get_selected_device(pv_speaker_t *object);

/**
* Gets the list of available audio devices that can be used for playing audio.
* Free the returned `device_list` array using `pv_speaker_free_device_list()`.
*
* @param[out] device_list_length The number of available audio devices.
* @param[out] device_list The output array containing the list of available audio devices.
* @return Status Code. Returns PV_SPEAKER_STATUS_OUT_OF_MEMORY, PV_SPEAKER_STATUS_BACKEND_ERROR or
* PV_SPEAKER_STATUS_INVALID_ARGUMENT on failure.
*/
PV_API pv_speaker_status_t pv_speaker_get_available_devices(
        int32_t *device_list_length,
        char ***device_list);

/**
* Frees the device list initialized by `pv_speaker_get_available_devices()`.
*
* @param device_list_length The number of audio devices.
* @param device_list The array containing the list of audio devices.
*/
PV_API void pv_speaker_free_available_devices(
        int32_t device_list_length,
        char **device_list);

/**
* Provides string representations of the given status code.
*
* @param status Status code.
* @return String representation.
*/
PV_API const char *pv_speaker_status_to_string(pv_speaker_status_t status);

/**
* Gets the PvSpeaker version.
*
* @return Version.
*/
PV_API const char *pv_speaker_version(void);

#endif //PV_SPEAKER_H