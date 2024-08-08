#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include "node_api.h"

#include "pv_speaker.h"

napi_value napi_pv_speaker_init(napi_env env, napi_callback_info info) {
    size_t argc = 4;
    napi_value args[argc];
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

    int32_t bits_per_sample;
    status = napi_get_value_int32(env, args[1], &bits_per_sample);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the bits per sample");
        return NULL;
    }

    int32_t buffer_size_secs;
    status = napi_get_value_int32(env, args[2], &buffer_size_secs);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the buffer size secs");
        return NULL;
    }

    int32_t device_index;
    status = napi_get_value_int32(env, args[3], &device_index);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the device index");
        return NULL;
    }

    pv_speaker_t *handle = NULL;
    pv_speaker_status_t pv_speaker_status = pv_speaker_init(
            sample_rate,
            (int16_t) bits_per_sample,
            buffer_size_secs,
            device_index,
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
    napi_value args[argc];
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
    napi_value args[argc];
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
    napi_value args[argc];
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
    size_t argc = 3;
    napi_value args[argc];
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

    int32_t bits_per_sample;
    status = napi_get_value_int32(env, args[1], &bits_per_sample);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the bits per sample");
        return NULL;
    }

    void* data = NULL;
    size_t byte_length = 0;
    status = napi_get_arraybuffer_info(env, args[2], &data, &byte_length);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get buffer");
        return NULL;
    }

    int32_t bytes_per_sample = bits_per_sample / 8;
    int32_t written_length = 0;
    pv_speaker_status_t pv_speaker_status = pv_speaker_write(
            (pv_speaker_t *)(uintptr_t) object_id,
            (int8_t *) data,
            (int32_t) (byte_length / bytes_per_sample),
            &written_length);

    napi_value object_js = NULL;
    napi_value status_js = NULL;
    napi_value written_length_js = NULL;
    const char *ERROR_MSG = "Unable to allocate memory for the write result";

    status = napi_create_object(env, &object_js);
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

    status = napi_create_int32(env, written_length, &written_length_js);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                ERROR_MSG);
        return NULL;
    }
    status = napi_set_named_property(env, object_js, "written_length", written_length_js);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                ERROR_MSG);
        return NULL;
    }

    return object_js;
}

napi_value napi_pv_speaker_flush(napi_env env, napi_callback_info info) {
    size_t argc = 3;
    napi_value args[argc];
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

    int32_t bits_per_sample;
    status = napi_get_value_int32(env, args[1], &bits_per_sample);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the bits per sample");
        return NULL;
    }

    void* data = NULL;
    size_t byte_length = 0;
    status = napi_get_arraybuffer_info(env, args[2], &data, &byte_length);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to get buffer");
        return NULL;
    }

    if (byte_length == 0) {
        int8_t val = 0;
        data = &val;
    }
    int32_t bytes_per_sample = bits_per_sample / 8;
    int32_t written_length = 0;
    pv_speaker_status_t pv_speaker_status = pv_speaker_flush(
            (pv_speaker_t *)(uintptr_t) object_id,
            (int8_t *) data,
            (int32_t) byte_length / bytes_per_sample,
            &written_length);

    napi_value object_js = NULL;
    napi_value status_js = NULL;
    napi_value written_length_js = NULL;
    const char *ERROR_MSG = "Unable to allocate memory for the flush result";

    status = napi_create_object(env, &object_js);
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

    status = napi_create_int32(env, written_length, &written_length_js);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                ERROR_MSG);
        return NULL;
    }
    status = napi_set_named_property(env, object_js, "written_length", written_length_js);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                ERROR_MSG);
        return NULL;
    }

    return object_js;
}

napi_value napi_pv_speaker_get_is_started(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[argc];
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

napi_value napi_pv_speaker_get_selected_device(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[argc];
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

napi_value napi_pv_speaker_write_to_file(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value args[argc];
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

    size_t length = 0;
    status = napi_get_value_string_utf8(env, args[1], NULL, 0, &length);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the output path");
        return NULL;
    }
    char *output_path = (char *) calloc(length + 1, sizeof(char));
    if (!output_path) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_OUT_OF_MEMORY),
                "Unable to allocate memory");
        return NULL;
    }
    status = napi_get_value_string_utf8(env, args[1], output_path, length + 1, &length);
    if (status != napi_ok) {
        free(output_path);
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT),
                "Unable to get the output path");
        return NULL;
    }

    pv_speaker_status_t pv_speaker_status = pv_speaker_write_to_file(
            (pv_speaker_t *)(uintptr_t) object_id, output_path);

    napi_value result;
    status = napi_create_int32(env, pv_speaker_status, &result);
    if (status != napi_ok) {
        napi_throw_error(
                env,
                pv_speaker_status_to_string(PV_SPEAKER_STATUS_RUNTIME_ERROR),
                "Unable to allocate memory for the write to file result");
        return NULL;
    }

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

    desc = DECLARE_NAPI_METHOD("flush", napi_pv_speaker_flush);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("get_is_started", napi_pv_speaker_get_is_started);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("get_selected_device", napi_pv_speaker_get_selected_device);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("get_available_devices", napi_pv_speaker_get_available_devices);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("write_to_file", napi_pv_speaker_write_to_file);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    desc = DECLARE_NAPI_METHOD("version", napi_pv_speaker_version);
    status = napi_define_properties(env, exports, 1, &desc);
    assert(status == napi_ok);

    return exports;
}

NAPI_MODULE(NODE_GYB_MODULE_NAME, Init)
