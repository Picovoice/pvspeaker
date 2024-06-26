# PvSpeaker Source Project

## Requirements

- CMake 3.4+.
- C99 compatible compiler.
- **Windows**: MinGW.

## Compatibility

- Linux (x86_64)
- macOS (x86_64, arm64)
- Windows (amd64)
- Raspberry Pi (3, 4 and 5)

## Compiling

Run the following commands to build and test (`{OUTPUT_DIR}` can be empty if you wish not to copy):

```console
git submodule update --init --recursive
cmake -S . -B build -DOUTPUT_DIR={OUTPUT_DIR} -DPV_SPEAKER_PLATFORM={PV_SPEAKER_PLATFORM}
cmake --build build
```

The variable `{OUTPUT_DIR}` will be used to select the directory to copy the shared object
after a successful compilation. `{OUTPUT_DIR}` should be a directory **relative** to the [lib](../lib) directory.

The `{PV_SPEAKER_PLATFORM}` variable will set the compilation flags for the given platform. Exclude this variable
to get a list of possible values.

## Usage

1. Create a PvSpeaker object:
```c
#include "pv_speaker.h"

uint32_t num_samples, sample_rate;
uint16_t bits_per_sample;
void *pcm = read_wav_file(input_wav_path, &num_samples, &sample_rate, &bits_per_sample);
    
const int32_t frame_length = 512;
const int32_t device_index = -1; // -1 == default device
const int32_t buffered_frame_count = 10;

pv_speaker_t *speaker = NULL;
pv_speaker_status_t status = pv_speaker_init(
        sample_rate,
        frame_length,
        bits_per_sample,
        device_index,
        10,
        &speaker);
if (status != PV_SPEAKER_STATUS_SUCCESS) {
    // handle PvSpeaker init error
}
```

2. Start the speaker:

```c
pv_speaker_status_t status = pv_speaker_start(speaker);
if (status != PV_SPEAKER_STATUS_SUCCESS) {
    // handle PvSpeaker start error
}
```

3. Read frames of audio from the speaker:
```c
// pcm from read_wav_file function
if (pcm) {
    // cast as char *
    char *pcmData = (char *) pcm;
    for (int i = 0; i < num_samples; i += frame_length) {
        // must have length equal to or less than `frame_length` that was given to `pv_speaker_init()`
        // be sure to handle the last frame properly (i.e. the last frame will likely not have length `frame_length`)
        bool is_last_frame = i + frame_length >= num_samples;
        int32_t last_frame_length = num_samples - i;

        status = pv_speaker_write(
                speaker,
                is_last_frame ? last_frame_length : frame_length,
                &pcmData[i * bits_per_sample / 8]);
        if (status != PV_SPEAKER_STATUS_SUCCESS) {
            // handle PvSpeaker write error
        }
    }

    free(pcm);
}
```

4. Stop playing:

```c
pv_speaker_status_t status = pv_speaker_stop(speaker);
if (status != PV_SPEAKER_STATUS_SUCCESS) {
    // handle PvSpeaker stop error
}
```

5. Release resources used by PvSpeaker:
```c
pv_speaker_delete(speaker);
```

### Selecting an Audio Device

To print a list of available audio devices:
```c
char **device_list = NULL;
int32_t device_list_length = 0;

pv_speaker_status_t status = pv_speaker_get_available_devices(&device_list_length, &device_list);
if (status != PV_SPEAKER_STATUS_SUCCESS) {
    // handle PvSpeaker get audio devices error
}

fprintf(stdout, "Printing devices...\n");
for (int32_t i = 0; i < device_list_length; i++) {
    fprintf(stdout, "index: %d, name: %s\n", i, device_list[i]);
}

pv_speaker_free_available_devices(device_list_length, device_list);
```

The index of the device in the returned list can be used in `pv_speaker_init()` to select that device for playing.

Refer to [pv_speaker_demo.c](../demo/c/pv_speaker_demo.c) for a full example of how to use `pv_speaker` to capture audio in C.
