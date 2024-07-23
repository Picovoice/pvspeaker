/*
    Copyright 2024 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#pragma GCC diagnostic push

#pragma GCC diagnostic ignored "-Wunused-result"

#define MINIAUDIO_IMPLEMENTATION

#include "miniaudio.h"

#pragma GCC diagnostic pop

#include "pv_circular_buffer.h"
#include "pv_speaker.h"

#define PV_SPEAKER_DEFAULT_DEVICE_INDEX (-1)

#define PV_SPEAKER_VERSION "1.0.0"

static bool is_flush_stopped = false;
static bool is_flushed_and_empty = false;
static bool is_data_requested_while_empty = false;

static const int32_t FLUSH_SLEEP_MS = 2;

struct pv_speaker {
    ma_context context;
    ma_device device;
    pv_circular_buffer_t *buffer;
    int32_t bits_per_sample;
    bool is_started;
    ma_mutex mutex;
};

static void pv_speaker_ma_callback(ma_device *device, void *output, const void *input, ma_uint32 frame_count) {
    (void) input;

    pv_speaker_t *object = (pv_speaker_t *) device->pUserData;

    ma_mutex_lock(&object->mutex);

    // this callback being invoked after calling `pv_speaker_flush` and the circular buffer is empty indicates that all
    // frames have been passed to the output buffer, and the device can stop without truncating the last frame of audio
    if (is_flushed_and_empty) {
        is_data_requested_while_empty = true;
        ma_mutex_unlock(&object->mutex);
        return;
    }

    int32_t read_length = 0;
    pv_circular_buffer_read(object->buffer, output, (int32_t) frame_count, &read_length);

    ma_mutex_unlock(&object->mutex);
}

PV_API pv_speaker_status_t pv_speaker_init(
        int32_t sample_rate,
        int16_t bits_per_sample,
        int32_t buffer_size_secs,
        int32_t device_index,
        pv_speaker_t **object) {
    if (device_index < PV_SPEAKER_DEFAULT_DEVICE_INDEX) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (sample_rate <= 0) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (bits_per_sample != 8 &&
        bits_per_sample != 16 &&
        bits_per_sample != 24 &&
        bits_per_sample != 32) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (buffer_size_secs <= 0) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (!object) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }

    *object = NULL;

    pv_speaker_t *o = calloc(1, sizeof(pv_speaker_t));
    if (!o) {
        return PV_SPEAKER_STATUS_OUT_OF_MEMORY;
    }

    ma_result result = ma_context_init(NULL, 0, NULL, &(o->context));
    if (result != MA_SUCCESS) {
        pv_speaker_delete(o);
        if ((result == MA_NO_BACKEND) || (result == MA_FAILED_TO_INIT_BACKEND)) {
            return PV_SPEAKER_STATUS_BACKEND_ERROR;
        } else if (result == MA_OUT_OF_MEMORY) {
            return PV_SPEAKER_STATUS_OUT_OF_MEMORY;
        } else {
            return PV_SPEAKER_STATUS_RUNTIME_ERROR;
        }
    }

    int16_t ma_format;
    switch (bits_per_sample) {
        case 8:
            ma_format = ma_format_u8;
            break;
        case 16:
            ma_format = ma_format_s16;
            break;
        case 24:
            ma_format = ma_format_s24;
            break;
        case 32:
            ma_format = ma_format_s32;
            break;
        default:
            ma_format = ma_format_unknown;
            break;
    }

    ma_device_config device_config;
    device_config = ma_device_config_init(ma_device_type_playback);
    device_config.playback.format = ma_format;
    device_config.playback.channels = MA_CHANNEL_MONO;
    device_config.sampleRate = sample_rate;
    device_config.dataCallback = pv_speaker_ma_callback;
    device_config.pUserData = o;

    if (device_index != PV_SPEAKER_DEFAULT_DEVICE_INDEX) {
        ma_device_info *playback_info = NULL;
        ma_uint32 count = 0;
        result = ma_context_get_devices(
                &(o->context),
                &playback_info,
                &count,
                NULL,
                NULL);
        if (result != MA_SUCCESS) {
            pv_speaker_delete(o);
            if (result == MA_OUT_OF_MEMORY) {
                return PV_SPEAKER_STATUS_OUT_OF_MEMORY;
            } else {
                return PV_SPEAKER_STATUS_RUNTIME_ERROR;
            }
        }
        if (count == 0) {
            pv_speaker_delete(o);
            return PV_SPEAKER_STATUS_RUNTIME_ERROR;
        }
        if (device_index >= count) {
            pv_speaker_delete(o);
            return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
        }
        device_config.playback.pDeviceID = &playback_info[device_index].id;
    }

    result = ma_device_init(&(o->context), &device_config, &(o->device));
    if (result != MA_SUCCESS) {
        pv_speaker_delete(o);
        if (result == MA_DEVICE_ALREADY_INITIALIZED) {
            return PV_SPEAKER_STATUS_DEVICE_ALREADY_INITIALIZED;
        } else if (result == MA_OUT_OF_MEMORY) {
            return PV_SPEAKER_STATUS_OUT_OF_MEMORY;
        } else {
            return PV_SPEAKER_STATUS_RUNTIME_ERROR;
        }
    }

    result = ma_mutex_init(&(o->mutex));
    if (result != MA_SUCCESS) {
        pv_speaker_delete(o);
        if (result == MA_OUT_OF_MEMORY) {
            return PV_SPEAKER_STATUS_OUT_OF_MEMORY;
        } else {
            return PV_SPEAKER_STATUS_RUNTIME_ERROR;
        }
    }

    const int32_t buffer_capacity = buffer_size_secs * sample_rate;
    const int32_t element_size = bits_per_sample / 8;
    pv_circular_buffer_status_t status = pv_circular_buffer_init(
            buffer_capacity,
            element_size,
            &(o->buffer));

    if (status != PV_CIRCULAR_BUFFER_STATUS_SUCCESS) {
        pv_speaker_delete(o);
        return PV_SPEAKER_STATUS_OUT_OF_MEMORY;
    }

    o->bits_per_sample = bits_per_sample;

    *object = o;

    return PV_SPEAKER_STATUS_SUCCESS;
}

PV_API void pv_speaker_delete(pv_speaker_t *object) {
    if (object) {
        ma_device_uninit(&(object->device));
        ma_context_uninit(&(object->context));
        ma_mutex_uninit(&(object->mutex));
        pv_circular_buffer_delete(object->buffer);
        free(object);
    }
}

PV_API pv_speaker_status_t pv_speaker_start(pv_speaker_t *object) {
    if (!object) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }

    is_flushed_and_empty = false;
    is_data_requested_while_empty = false;

    ma_result result = ma_device_start(&(object->device));
    if (result != MA_SUCCESS) {
        if (result == MA_DEVICE_NOT_INITIALIZED) {
            return PV_SPEAKER_STATUS_DEVICE_NOT_INITIALIZED;
        } else {
            // device already started
            return PV_SPEAKER_STATUS_INVALID_STATE;
        }
    }

    object->is_started = true;

    return PV_SPEAKER_STATUS_SUCCESS;
}

PV_API pv_speaker_status_t pv_speaker_write(pv_speaker_t *object, int8_t *pcm, int32_t pcm_length, int32_t *written_length) {
    if (!object) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (!pcm) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (pcm_length <= 0) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (!written_length) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (!(object->is_started)) {
        return PV_SPEAKER_STATUS_INVALID_STATE;
    }

    is_flush_stopped = false;

    ma_mutex_lock(&object->mutex);

    int32_t available = 0;
    pv_circular_buffer_status_t status = pv_circular_buffer_get_available(object->buffer, &available);
    if (status != PV_CIRCULAR_BUFFER_STATUS_SUCCESS) {
        ma_mutex_unlock(&object->mutex);
        return PV_SPEAKER_STATUS_RUNTIME_ERROR;
    }

    int32_t to_write = pcm_length < available ? pcm_length : available;
    if (to_write > 0) {
        status = pv_circular_buffer_write(object->buffer, pcm, to_write);
        if (status != PV_CIRCULAR_BUFFER_STATUS_SUCCESS) {
            ma_mutex_unlock(&object->mutex);
            return PV_SPEAKER_STATUS_RUNTIME_ERROR;
        }
    }

    *written_length = to_write;

    ma_mutex_unlock(&object->mutex);

    return PV_SPEAKER_STATUS_SUCCESS;
}

PV_API pv_speaker_status_t pv_speaker_flush(pv_speaker_t *object, int8_t *pcm, int32_t pcm_length, int32_t *written_length) {
    if (!object) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (!pcm) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (pcm_length < 0) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (!written_length) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (!(object->is_started)) {
        return PV_SPEAKER_STATUS_INVALID_STATE;
    }

    int32_t written = 0;
    *written_length = 0;

    while (!is_flush_stopped && written < pcm_length) {
        ma_mutex_lock(&object->mutex);

        int32_t available = 0;
        pv_circular_buffer_status_t status = pv_circular_buffer_get_available(object->buffer, &available);
        if (status != PV_CIRCULAR_BUFFER_STATUS_SUCCESS) {
            ma_mutex_unlock(&object->mutex);
            return PV_SPEAKER_STATUS_RUNTIME_ERROR;
        }

        int32_t remaining = pcm_length - written;
        int32_t to_write = remaining < available ? remaining : available;
        if (to_write > 0) {
            status = pv_circular_buffer_write(
                    object->buffer,
                    &pcm[written * object->bits_per_sample / 8],
                    to_write);
            if (status == PV_CIRCULAR_BUFFER_STATUS_SUCCESS) {
                written += to_write;
                *written_length = written;
            }
        }

        ma_mutex_unlock(&object->mutex);
        ma_sleep(FLUSH_SLEEP_MS);
    }

    // waits for all frames to be copied to output buffer
    while (!is_flush_stopped && !is_data_requested_while_empty) {
        ma_mutex_lock(&object->mutex);

        int32_t count = 0;
        pv_circular_buffer_status_t status = pv_circular_buffer_get_count(object->buffer, &count);
        if (status == PV_CIRCULAR_BUFFER_STATUS_SUCCESS && count == 0) {
            is_flushed_and_empty = true;
        } else if (status != PV_CIRCULAR_BUFFER_STATUS_SUCCESS) {
            ma_mutex_unlock(&object->mutex);
            return PV_SPEAKER_STATUS_RUNTIME_ERROR;
        }

        ma_mutex_unlock(&object->mutex);
        ma_sleep(FLUSH_SLEEP_MS);
    }

    return PV_SPEAKER_STATUS_SUCCESS;
}

PV_API void pv_speaker_stop_flush() {
    is_flush_stopped = true;
}

PV_API pv_speaker_status_t pv_speaker_stop(pv_speaker_t *object) {
    if (!object) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }

    ma_result result = ma_device_stop(&(object->device));
    if (result != MA_SUCCESS) {
        if (result == MA_DEVICE_NOT_INITIALIZED) {
            return PV_SPEAKER_STATUS_DEVICE_NOT_INITIALIZED;
        } else {
            // device already stopped
            return PV_SPEAKER_STATUS_INVALID_STATE;
        }
    }

    ma_mutex_lock(&object->mutex);
    pv_circular_buffer_reset(object->buffer);
    object->is_started = false;
    ma_mutex_unlock(&object->mutex);

    return PV_SPEAKER_STATUS_SUCCESS;
}

PV_API bool pv_speaker_get_is_started(pv_speaker_t *object) {
    if (!object) {
        return false;
    }
    return object->is_started;
}

PV_API const char *pv_speaker_get_selected_device(pv_speaker_t *object) {
    if (!object) {
        return NULL;
    }
    return object->device.playback.name;
}

PV_API pv_speaker_status_t pv_speaker_get_available_devices(
        int32_t *device_list_length,
        char ***device_list) {
    if (!device_list_length) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }
    if (!device_list) {
        return PV_SPEAKER_STATUS_INVALID_ARGUMENT;
    }

    ma_context context;
    ma_result result = ma_context_init(NULL, 0, NULL, &context);
    if (result != MA_SUCCESS) {
        if ((result == MA_NO_BACKEND) || (result == MA_FAILED_TO_INIT_BACKEND)) {
            return PV_SPEAKER_STATUS_BACKEND_ERROR;
        } else if (result == MA_OUT_OF_MEMORY) {
            return PV_SPEAKER_STATUS_OUT_OF_MEMORY;
        } else {
            return PV_SPEAKER_STATUS_INVALID_STATE;
        }
    }

    ma_device_info *playback_info;
    ma_uint32 playback_count;
    result = ma_context_get_devices(
            &context,
            NULL,
            NULL,
            &playback_info,
            &playback_count);
    if (result != MA_SUCCESS) {
        ma_context_uninit(&context);
        if (result == MA_OUT_OF_MEMORY) {
            return PV_SPEAKER_STATUS_OUT_OF_MEMORY;
        } else {
            return PV_SPEAKER_STATUS_INVALID_STATE;
        }
    }

    char **d = calloc(playback_count, sizeof(char *));
    if (!d) {
        ma_context_uninit(&context);
        return PV_SPEAKER_STATUS_OUT_OF_MEMORY;
    }

    for (int32_t i = 0; i < (int32_t) playback_count; i++) {
        d[i] = strdup(playback_info[i].name);
        if (!d[i]) {
            for (int32_t j = i - 1; j >= 0; j--) {
                free(d[j]);
            }
            free(d);
            ma_context_uninit(&context);
            return PV_SPEAKER_STATUS_OUT_OF_MEMORY;
        }
    }

    ma_context_uninit(&context);

    *device_list_length = (int32_t) playback_count;
    *device_list = d;

    return PV_SPEAKER_STATUS_SUCCESS;
}

PV_API void pv_speaker_free_available_devices(
        int32_t device_list_length,
        char **device_list) {
    if (device_list && (device_list_length > 0)) {
        for (int32_t i = 0; i < device_list_length; i++) {
            free(device_list[i]);
        }
        free(device_list);
    }
}

PV_API const char *pv_speaker_status_to_string(pv_speaker_status_t status) {
    static const char *const STRINGS[] = {
            "SUCCESS",
            "OUT_OF_MEMORY",
            "INVALID_ARGUMENT",
            "INVALID_STATE",
            "BACKEND_ERROR",
            "DEVICE_INITIALIZED",
            "DEVICE_NOT_INITIALIZED",
            "IO_ERROR",
            "RUNTIME_ERROR"};

    int32_t size = sizeof(STRINGS) / sizeof(STRINGS[0]);
    if (status < PV_SPEAKER_STATUS_SUCCESS || status >= (PV_SPEAKER_STATUS_SUCCESS + size)) {
        return NULL;
    }

    return STRINGS[status - PV_SPEAKER_STATUS_SUCCESS];
}

PV_API const char *pv_speaker_version(void) {
    return PV_SPEAKER_VERSION;
}
