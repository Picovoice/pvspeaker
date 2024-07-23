/*
    Copyright 2024 Picovoice Inc.

    You may not use this file except in compliance with the license. A copy of the license is located in the "LICENSE"
    file accompanying this source.

    Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
    an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
    specific language governing permissions and limitations under the License.
*/

#include <getopt.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pv_speaker.h"

static volatile bool is_interrupted = false;

void interrupt_handler(int _) {
    (void) _;
    is_interrupted = true;
    pv_speaker_stop_flush();
    fprintf(stdout, "\nStopped...\n");
}

static struct option long_options[] = {
        {"show_audio_devices", no_argument,       NULL, 's'},
        {"input_wav_path",     required_argument, NULL, 'i'},
        {"audio_device_index", required_argument, NULL, 'd'},
        {"buffer_size_secs",   required_argument, NULL, 'b'}
};

static void print_usage(const char *program_name) {
    fprintf(stderr,
            "Usage : %s -i INPUT_WAV_PATH [-d AUDIO_DEVICE_INDEX] [-b BUFFER_SIZE_SECS]\n"
            "        %s --show_audio_devices\n",
            program_name,
            program_name);
}

static void show_audio_devices(void) {
    char **device_list = NULL;
    int32_t device_list_length = 0;

    // List devices
    pv_speaker_status_t status = pv_speaker_get_available_devices(
            &device_list_length,
            &device_list);
    if (status != PV_SPEAKER_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to get audio devices with: %s.\n", pv_speaker_status_to_string(status));
        exit(1);
    }

    fprintf(stdout, "Printing devices...\n");
    for (int32_t i = 0; i < device_list_length; i++) {
        fprintf(stdout, "index: %d, name: %s\n", i, device_list[i]);
    }

    pv_speaker_free_available_devices(device_list_length, device_list);
}

typedef struct {
    uint8_t chunk_id[4];
    uint32_t chunk_size;
    uint8_t format[4];
    uint8_t subchunk1_id[4];
    uint32_t subchunk1_size;
    uint16_t audio_format;
    uint16_t num_channels;
    uint32_t sample_rate;
    uint32_t byte_rate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    uint8_t subchunk2_id[4];
    uint32_t subchunk2_size;
} wav_header;

void *read_wav_file(const char *filename, uint32_t *num_samples, uint32_t *sample_rate, uint16_t *bits_per_sample) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Unable to open file");
        return NULL;
    }

    wav_header header;

    fread(&header, sizeof(header), 1, file);

    if (header.chunk_id[0] != 'R' || header.chunk_id[1] != 'I' || header.chunk_id[2] != 'F' || header.chunk_id[3] != 'F' ||
        header.format[0] != 'W' || header.format[1] != 'A' || header.format[2] != 'V' || header.format[3] != 'E') {
        fclose(file);
        fprintf(stderr, "Invalid WAV file\n");
        return NULL;
    }

    if (header.audio_format != 1) {
        fclose(file);
        fprintf(stderr, "WAV file format must be PCM type\n");
        return NULL;
    }

    if (header.num_channels != 1) {
        fclose(file);
        fprintf(stderr, "WAV file must have a single channel (MONO)\n");
        return NULL;
    }

    *sample_rate = header.sample_rate;
    *bits_per_sample = header.bits_per_sample;
    uint32_t bytes_per_sample = header.bits_per_sample / 8;
    *num_samples = header.subchunk2_size / bytes_per_sample;

    void *pcm_data = malloc(header.subchunk2_size);

    if (!pcm_data) {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }

    fread(pcm_data, header.subchunk2_size, 1, file);

    fclose(file);

    return pcm_data;
}

int main(int argc, char *argv[]) {
    const char *input_wav_path = NULL;
    int32_t device_index = -1;
    int32_t buffer_size_secs = 20;

    int c;
    while ((c = getopt_long(argc, argv, "si:d:b:", long_options, NULL)) != -1) {
        switch (c) {
            case 's':
                show_audio_devices();
                return 0;
            case 'i':
                input_wav_path = optarg;
                break;
            case 'd':
                device_index = (int32_t) strtol(optarg, NULL, 10);
                break;
            case 'b':
                buffer_size_secs = (int32_t) strtol(optarg, NULL, 10);
                break;
            default:
                exit(1);
        }
    }

    if (!input_wav_path) {
        print_usage(argv[0]);
        exit(1);
    }

    signal(SIGINT, interrupt_handler);
    fprintf(stdout, "pv_speaker version: %s\n", pv_speaker_version());

    uint32_t num_samples, sample_rate;
    uint16_t bits_per_sample;
    void *pcm_data = read_wav_file(input_wav_path, &num_samples, &sample_rate, &bits_per_sample);

    fprintf(stdout, "Initializing pv_speaker...\n");
    pv_speaker_t *speaker = NULL;
    pv_speaker_status_t status = pv_speaker_init(
            sample_rate,
            bits_per_sample,
            buffer_size_secs,
            device_index,
            &speaker);
    if (status != PV_SPEAKER_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to initialize device with %s.\n", pv_speaker_status_to_string(status));
        exit(1);
    }

    const char *selected_device = pv_speaker_get_selected_device(speaker);
    fprintf(stdout, "Selected device: %s.\n", selected_device);

    status = pv_speaker_start(speaker);
    if (status != PV_SPEAKER_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to start device with %s.\n", pv_speaker_status_to_string(status));
        exit(1);
    }

    fprintf(stdout, "Playing audio...\n");
    if (pcm_data) {
        int8_t *pcm = (int8_t *) pcm_data;
        int32_t total_written_length = 0;

        while (!is_interrupted && total_written_length < num_samples) {
            int32_t written_length = 0;
            status = pv_speaker_write(
                    speaker,
                    &pcm[total_written_length * bits_per_sample / 8],
                    num_samples - total_written_length,
                    &written_length);
            if (status != PV_SPEAKER_STATUS_SUCCESS) {
                fprintf(stderr, "Failed to write with %s.\n", pv_speaker_status_to_string(status));
                exit(1);
            }
            total_written_length += written_length;
        }

        free(pcm);
    }

    fprintf(stdout, "Waiting for audio to finish...\n");
    int32_t pcm_length = 0;
    int16_t pcm[pcm_length];
    int8_t *pcm_ptr = (int8_t *) pcm;
    int32_t written_length = 0;
    status = pv_speaker_flush(speaker, pcm_ptr, pcm_length, &written_length);
    if (status != PV_SPEAKER_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to flush pcm with %s.\n", pv_speaker_status_to_string(status));
        exit(1);
    }

    if (!is_interrupted) {
        fprintf(stdout, "Finished playing audio...\n");
    }

    status = pv_speaker_stop(speaker);
    if (status != PV_SPEAKER_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to stop device with %s.\n", pv_speaker_status_to_string(status));
        exit(1);
    }

    fprintf(stdout, "Deleting pv_speaker...\n");
    pv_speaker_delete(speaker);

    return 0;
}
