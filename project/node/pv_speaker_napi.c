#include <assert.h>
#include <string.h>
#include "node_api.h"

#include "pv_speaker.h"

napi_value napi_pv_speaker_init(napi_env env, napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5];
    napi_status status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get input arguments");
        return NULL;
    }

    int32_t sample_rate;
    status = napi_get_value_int32(env, args[0], &sample_rate);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the sample rate");
        return NULL;
    }

option(
    "-i, --audio_device_index <number>",
    "index of audio device to use to play audio",
    Number,
    -1
  ).option    int32_t bits_per_sample;
    status = napi_get_value_int32(env, args[1], &bits_per_sample);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the bits per sample");
        return NULL;
    }

    int32_t device_index;
    status = napi_get_value_int32(env, args[2], &device_index);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the device index");
        return NULL;
    }

    int32_t frame_length;
    status = napi_get_value_int32(env, args[3], &frame_length);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the frame length");
        return NULL;
    }

    int32_t buffered_frames_count;
    status = napi_get_value_int32(env, args[4], &buffered_frames_count);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the buffered frames count");
        return NULL;
    }

    pv_speaker_t *handle = NULL;
    pv_speaker_status_t pv_speaker_status = pv_speaker_init(
            sample_rate,
            frame_length,
            (int16_t) bits_per_sample,
            device_index,
            buffered_frames_count,
            &handle);
    if (pv_speaker_status != PV_SPEAKER_STATUS_SUCCESS) {
        handle = NULL;
    }

    napi_value object_js = NULL;
    napi_value handle_js = NULL;
    napi_value status_js = NULL;
    const char *ERROR_MSG = "Unable to allocate memory for the constructed instance of PvSpeaker";

    status = napi_create_object(env, &object_js);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                ERROR_MSG);
        return NULL;
    }

    status = napi_create_bigint_uint64(env, ((uint64_t) handle), &handle_js);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                ERROR_MSG);
        return NULL;
    }
    status = napi_set_named_property(env, object_js, "handle", handle_js);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                ERROR_MSG);
        return NULL;
    }
    status = napi_create_int32(env, pv_speaker_status, &status_js);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                ERROR_MSG);
        return NULL;
    }
    status = napi_set_named_property(env, object_js, "status", status_js);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                ERROR_MSG);
        return NULL;
    }

    return object_js;
}

napi_value napi_pv_speaker_delete(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_status status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get input arguments");
        return NULL;
    }

    uint64_t object_id = 0;
    bool lossless = false;
    status = napi_get_value_bigint_uint64(env, args[0], &object_id, &lossless);
    if ((status != napi_ok) || !lossless) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get the address of the instance of PvSpeaker properly");
        return NULL;
    }

    pv_speaker_delete((pv_speaker_t *)(uintptr_t) object_id);
    return NULL;
}

napi_value napi_pv_speaker_start(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_status status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get input arguments");
        return NULL;
    }

    uint64_t object_id = 0;
    bool lossless = false;
    status = napi_get_value_bigint_uint64(env, args[0], &object_id, &lossless);
    if ((status != napi_ok) || !lossless) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get the address of the instance of PvSpeaker properly");
        return NULL;
    }

    pv_speaker_status_t pv_speaker_status = pv_speaker_start((pv_speaker_t *)(uintptr_t) object_id);

    napi_value result;
    status = napi_create_int32(env, pv_speaker_status, &result);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to allocate memory for the start result");
        return NULL;
    }

    return result;
}

napi_value napi_pv_speaker_stop(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_status status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get input arguments");
        return NULL;
    }

    uint64_t object_id = 0;
    bool lossless = false;
    status = napi_get_value_bigint_uint64(env, args[0], &object_id, &lossless);
    if ((status != napi_ok) || !lossless) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get the address of the instance of PvSpeaker properly");
        return NULL;
    }

    pv_speaker_status_t pv_speaker_status = pv_speaker_stop((pv_speaker_t *)(uintptr_t) object_id);

    napi_value result;
    status = napi_create_int32(env, pv_speaker_status, &result);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to allocate memory for the stop result");
        return NULL;
    }

    return result;
}

napi_value napi_pv_speaker_write(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];
    napi_status status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get input arguments");
        return NULL;
    }

    uint64_t object_id = 0;
    bool lossless = false;
    status = napi_get_value_bigint_uint64(env, args[0], &object_id, &lossless);
    if ((status != napi_ok) || !lossless) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get the address of the instance of PvSpeaker properly");
        return NULL;
    }

    napi_typedarray_type arr_type = -1;
    size_t length = 0;
    void* pcm_data = NULL;
    napi_value arraybuffer = NULL;
    size_t byte_offset = 0;
    status = napi_get_typedarray_info(env, args[1], &arr_type, &length, &pcm_data, &arraybuffer, &byte_offset);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get typedarray");
        return NULL;
    }
    if (arr_type != napi_uint8_array && arr_type != napi_int16_array && arr_type != napi_int32_array) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Invalid type of input pcm buffer. The input frame has to be 'Uint8Array', 'Int16Array', or 'Int32Array'");
        return NULL;
    }

    size_t buffer_length = 0;
    if (arr_type == napi_uint8_array) {
        buffer_length = length * sizeof(uint8_t);
    } else if (arr_type == napi_int16_array) {
        buffer_length = length * sizeof(int16_t);
    } else if (arr_type == napi_int32_array) {
        buffer_length = length * sizeof(int32_t);
    }

    void* data = NULL;
    napi_value res = NULL;
    status = napi_create_buffer(env, buffer_length, &data, &res);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get frame");
        return NULL;
    }
    memcpy(data, pcm_data, buffer_length);

    pv_speaker_status_t pv_speaker_status = pv_speaker_write(
            (pv_speaker_t *)(uintptr_t) object_id, (int32_t) length, data);

    napi_value result;
    status = napi_create_int32(env, pv_speaker_status, &result);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to allocate memory for the write result");
        return NULL;
    }

    return result;
}

napi_value napi_pv_speaker_get_is_started(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_status status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get input arguments");
        return NULL;
    }

    uint64_t object_id = 0;
    bool lossless = false;
    status = napi_get_value_bigint_uint64(env, args[0], &object_id, &lossless);
    if ((status != napi_ok) || !lossless) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get the address of the instance of PvSpeaker properly");
        return NULL;
    }

    bool is_started = pv_speaker_get_is_started((pv_speaker_t *)(uintptr_t) object_id);

    napi_value result;
    status = napi_get_boolean(env, is_started, &result);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get is started flag.");
        return NULL;
    }

    return result;
}

napi_value napi_pv_speaker_set_debug_logging(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2];
    napi_status status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get input arguments");
        return NULL;
    }

    uint64_t object_id = 0;
    bool lossless = false;
    status = napi_get_value_bigint_uint64(env, args[0], &object_id, &lossless);
    if ((status != napi_ok) || !lossless) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get the address of the instance of PvSpeaker properly");
        return NULL;
    }

    bool is_debug_logging_enabled;
    status = napi_get_value_bool(env, args[1], &is_debug_logging_enabled);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get debug logging flag");
        return NULL;
    }

    pv_speaker_set_debug_logging((pv_speaker_t *)(uintptr_t) object_id, is_debug_logging_enabled);
    return NULL;
}


napi_value napi_pv_speaker_get_selected_device(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1];
    napi_status status = napi_get_cb_info(env, info, &argc, args, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get input arguments");
        return NULL;
    }

    uint64_t object_id = 0;
    bool lossless = false;
    status = napi_get_value_bigint_uint64(env, args[0], &object_id, &lossless);
    if ((status != napi_ok) || !lossless) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get the address of the instance of PvSpeaker properly");
        return NULL;
    }

    napi_value result;
    status = napi_create_string_utf8(env, pv_speaker_get_selected_device((pv_speaker_t *)(uintptr_t) object_id), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to allocate memory for the write result");
        return NULL;
    }

    return result;
}

napi_value napi_pv_speaker_get_available_devices(napi_env env, napi_callback_info info) {
    size_t argc = 0;
    napi_status status = napi_get_cb_info(env, info, &argc, NULL, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get input arguments");
        return NULL;
    }

    int32_t device_list_length = 0;
    char **device_list = NULL;
    pv_speaker_status_t pv_speaker_status = pv_speaker_get_available_devices(&device_list_length, &device_list);

    if (pv_speaker_status != PV_SPEAKER_STATUS_SUCCESS) {
        return NULL;
    }

    napi_value result;
    status = napi_create_array_with_length(env, device_list_length, &result);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to allocate memory for the devices result");
        return NULL;
    }

    for (int32_t i = 0; i < device_list_length; i++) {
        napi_value device_name;
        status = napi_create_string_utf8(env, device_list[i], NAPI_AUTO_LENGTH, &device_name);
        if (status != napi_ok) {
            napi_throw_error(
                    env,
                    pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                    "Unable to allocate memory for the device name");
            return NULL;
        }

        status = napi_set_element(env, result, i, device_name);
        if (status != napi_ok) {
            napi_throw_error(
                    env,
                    pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                    "Unable to copy device name to allocated memory");
            return NULL;
        }
    }

    pv_speaker_free_available_devices(device_list_length, device_list);
    return result;
}

napi_value napi_pv_speaker_version(napi_env env, napi_callback_info info) {
    (void)(info);

    napi_value result;
    napi_status status = napi_create_string_utf8(env, pv_speaker_version(), NAPI_AUTO_LENGTH, &result);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to allocate memory for the version result");
        return NULL;
    }

    return result;
}

#define DECLARE_NAPI_METHOD(name, func) (napi_property_descriptor){ name, 0, func, 0, 0, 0, napi_default, 0 }

napi_value Init(napi_env env, napi_value exports) {
    napi_property_descriptor desc = DECLARE_NAPI_METHOD("init", napi_pv_speaker_init);
    napi_status status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("delete", napi_pv_speaker_delete);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("start", napi_pv_speaker_start);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("stop", napi_pv_speaker_stop);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("write", napi_pv_speaker_write);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("get_is_started", napi_pv_speaker_get_is_started);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("set_debug_logging", napi_pv_speaker_set_debug_logging);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("get_selected_device", napi_pv_speaker_get_selected_device);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("get_available_devices", napi_pv_speaker_get_available_devices);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("version", napi_pv_speaker_version);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    return exports;
}

NAPI_MODULE(NODE_GYB_MODULE_NAME, Init)
