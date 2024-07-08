/*
    Copyright 2024 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include "string.h"

#include "pv_speaker.h"
#include "test_helper.h"

static void init_test_helper(
        int32_t sample_rate,
        int32_t frame_length,
        int16_t bits_per_sample,
        int32_t device_index,
        int32_t buffered_frames_count,
        pv_speaker_status_t expected_status) {
    pv_speaker_t *speaker = NULL;
    pv_speaker_status_t status;

    status = pv_speaker_init(
            sample_rate,
            frame_length,
            bits_per_sample,
            device_index,
            buffered_frames_count,
            &speaker);

    check_condition(
            status == expected_status,
            __FUNCTION__,
            __LINE__,
            "Speaker initialization returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(expected_status));
    if (speaker) {
        pv_speaker_delete(speaker);
    }
}

static void test_pv_speaker_init(void) {
    printf("Initialize with valid parameters\n");
    init_test_helper(16000, 512, 16, 0, 10, PV_SPEAKER_STATUS_SUCCESS);

    printf("Initialize with valid parameters (different sample rate)\n");
    init_test_helper(22050, 512, 16, 0, 10, PV_SPEAKER_STATUS_SUCCESS);

    printf("Initialize with valid parameters (different frame length)\n");
    init_test_helper(16000, 256, 16, 0, 10, PV_SPEAKER_STATUS_SUCCESS);

    printf("Initialize with valid parameters (different bits per sample)\n");
    init_test_helper(16000, 512, 8, 0, 10, PV_SPEAKER_STATUS_SUCCESS);

    printf("Initialize with invalid device index (negative)\n");
    init_test_helper(16000, 512, 16, -2, 10, PV_SPEAKER_STATUS_INVALID_ARGUMENT);

    printf("Initialize with invalid device index (too high)\n");
    init_test_helper(16000, 512, 16, 500, 10, PV_SPEAKER_STATUS_INVALID_ARGUMENT);

    printf("Initialize with invalid frame length\n");
    init_test_helper(16000, -1, 16, 0, 10, PV_SPEAKER_STATUS_INVALID_ARGUMENT);

    printf("Initialize with invalid bits per sample\n");
    init_test_helper(16000, 512, -1, -2, 10, PV_SPEAKER_STATUS_INVALID_ARGUMENT);

    printf("Initialize with invalid buffered frames count\n");
    init_test_helper(16000, 512, 16, 0, 0, PV_SPEAKER_STATUS_INVALID_ARGUMENT);

    printf("Initialize with null speaker pointer\n");
    pv_speaker_status_t status = pv_speaker_init(16000, 512, 16, 0, 10, NULL);
    check_condition(
            status == PV_SPEAKER_STATUS_INVALID_ARGUMENT,
            __FUNCTION__,
            __LINE__,
            "Speaker initialization returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT));
}

static void test_pv_speaker_start_stop(void) {
    pv_speaker_t *speaker = NULL;
    pv_speaker_status_t status;
    int32_t frame_length = 512;
    int16_t frame[frame_length];
    char *frame_ptr = (char *) frame;

    status = pv_speaker_init(16000, frame_length, 16, 0, 10, &speaker);
    check_condition(
            status == PV_SPEAKER_STATUS_SUCCESS,
            __FUNCTION__,
            __LINE__,
            "Speaker initialization returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_SUCCESS));

    printf("Check is_started on NULL\n");
    bool is_started = pv_speaker_get_is_started(NULL);
    check_condition(
            is_started == false,
            __FUNCTION__,
            __LINE__,
            "get_is_started returned true on a NULL object.");

    printf("Check is_started on before start\n");
    is_started = pv_speaker_get_is_started(speaker);
    check_condition(
            is_started == false,
            __FUNCTION__,
            __LINE__,
            "get_is_started returned true - expected false.");

    printf("Call start on null object\n");
    status = pv_speaker_start(NULL);
    check_condition(
            status == PV_SPEAKER_STATUS_INVALID_ARGUMENT,
            __FUNCTION__,
            __LINE__,
            "Speaker start returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT));

    printf("Call read before start object\n");
    status = pv_speaker_write(speaker, frame_length, frame_ptr);
    check_condition(
            status == PV_SPEAKER_STATUS_INVALID_STATE,
            __FUNCTION__,
            __LINE__,
            "Speaker read returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_STATE));

    printf("Call start on valid speaker object\n");
    status = pv_speaker_start(speaker);
    check_condition(
            status == PV_SPEAKER_STATUS_SUCCESS,
            __FUNCTION__,
            __LINE__,
            "Speaker start returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_SUCCESS));

    printf("Call write on null speaker\n");
    status = pv_speaker_write(NULL, frame_length, frame_ptr);
    check_condition(
            status == PV_SPEAKER_STATUS_INVALID_ARGUMENT,
            __FUNCTION__,
            __LINE__,
            "Speaker write returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT));

    printf("Call write with null frame\n");
    status = pv_speaker_write(speaker, frame_length, NULL);
    check_condition(
            status == PV_SPEAKER_STATUS_INVALID_ARGUMENT,
            __FUNCTION__,
            __LINE__,
            "Speaker write returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT));

    printf("Call write with valid args\n");
    status = pv_speaker_write(speaker, frame_length, frame_ptr);
    check_condition(
            status == PV_SPEAKER_STATUS_SUCCESS,
            __FUNCTION__,
            __LINE__,
            "Speaker write returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_SUCCESS));

    printf("Check is_started on started speaker\n");
    is_started = pv_speaker_get_is_started(speaker);
    check_condition(
            is_started == true,
            __FUNCTION__,
            __LINE__,
            "get_is_started returned false - expected true.");

    printf("Call stop on null speaker object\n");
    status = pv_speaker_stop(NULL);
    check_condition(
            status == PV_SPEAKER_STATUS_INVALID_ARGUMENT,
            __FUNCTION__,
            __LINE__,
            "Speaker stop returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT));

    printf("Call stop on valid speaker object\n");
    status = pv_speaker_stop(speaker);
    check_condition(
            status == PV_SPEAKER_STATUS_SUCCESS,
            __FUNCTION__,
            __LINE__,
            "Speaker stop returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_SUCCESS));

    printf("Check is_started on stopped speaker\n");
    is_started = pv_speaker_get_is_started(speaker);
    check_condition(
            is_started == false,
            __FUNCTION__,
            __LINE__,
            "get_is_started returned true - expected false.");

    pv_speaker_delete(speaker);
}

static void test_pv_speaker_set_debug_logging(void) {
    pv_speaker_t *speaker = NULL;
    pv_speaker_status_t status = pv_speaker_init(16000, 512, 16, 0, 10, &speaker);
    check_condition(
            status == PV_SPEAKER_STATUS_SUCCESS,
            __FUNCTION__,
            __LINE__,
            "Speaker initialization returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_SUCCESS));

    pv_speaker_set_debug_logging(NULL, true);
    pv_speaker_set_debug_logging(speaker, true);

    pv_speaker_delete(speaker);
}

static void test_pv_speaker_get_selected_device(void) {
    pv_speaker_t *speaker = NULL;
    pv_speaker_status_t status = pv_speaker_init(16000, 512, 16, 0, 10, &speaker);
    check_condition(
            status == PV_SPEAKER_STATUS_SUCCESS,
            __FUNCTION__,
            __LINE__,
            "Speaker initialization returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_SUCCESS));

    check_condition(
            pv_speaker_get_selected_device(NULL) == NULL,
            __FUNCTION__,
            __LINE__,
            "pv_speaker_get_selected_device should have returned NULL.");

    check_condition(
            strcmp(pv_speaker_get_selected_device(speaker), "") != 0,
            __FUNCTION__,
            __LINE__,
            "pv_speaker_get_selected_device should have returned a device name");

    pv_speaker_delete(speaker);
}

static void test_pv_speaker_get_available_devices(void) {
    pv_speaker_status_t status;
    int32_t device_list_length = -1;
    char **device_list = NULL;

    status = pv_speaker_get_available_devices(NULL, &device_list);
    check_condition(
            status == PV_SPEAKER_STATUS_INVALID_ARGUMENT,
            __FUNCTION__,
            __LINE__,
            "pv_speaker_get_available_devices returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT));

    status = pv_speaker_get_available_devices(&device_list_length, NULL);
    check_condition(
            status == PV_SPEAKER_STATUS_INVALID_ARGUMENT,
            __FUNCTION__,
            __LINE__,
            "pv_speaker_get_available_devices returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_INVALID_ARGUMENT));

    status = pv_speaker_get_available_devices(&device_list_length, &device_list);
    check_condition(
            status == PV_SPEAKER_STATUS_SUCCESS,
            __FUNCTION__,
            __LINE__,
            "pv_speaker_get_available_devices returned %s - expected %s.",
            pv_speaker_status_to_string(status),
            pv_speaker_status_to_string(PV_SPEAKER_STATUS_SUCCESS));
    check_condition(
            device_list_length >= 0,
            __FUNCTION__,
            __LINE__,
            "device_list_length should have been greater than 0, instead got %d",
            device_list_length);
    check_condition(
            device_list != NULL,
            __FUNCTION__,
            __LINE__,
            "device_list should have not been NULL");

    pv_speaker_free_available_devices(device_list_length, device_list);
}

static void test_pv_speaker_version(void) {
    const char *version = pv_speaker_version();
    check_condition(
            strcmp(version, "") != 0,
            __FUNCTION__,
            __LINE__,
            "Version was supposed to be a non-empty string.");
}

int main() {
    srand(time(NULL));

    test_pv_speaker_get_available_devices();
    test_pv_speaker_version();
    test_pv_speaker_init();
    test_pv_speaker_start_stop();
    test_pv_speaker_set_debug_logging();
    test_pv_speaker_get_selected_device();

    return 0;
}