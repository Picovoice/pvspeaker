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

const int32_t sample_rate = 22050;
const int16_t bits_per_sample = 16;
const int32_t buffer_size_secs = 20;
const int32_t device_index = -1; // -1 == default device

pv_speaker_t *speaker = NULL;
pv_speaker_status_t status = pv_speaker_init(
        sample_rate,
        bits_per_sample,
        buffer_size_secs,
        device_index,
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

3. Write pcm data to the speaker:
```c
int32_3 num_samples;
int8_t *pcm = get_pcm_data(&num_samples);

status = pv_speaker_write(speaker, pcm, num_samples);
if (status != PV_SPEAKER_STATUS_SUCCESS) {
    // handle PvSpeaker start error
}
```

4. Wait for buffered audio to finish playing:

```c
pv_speaker_status_t status = pv_speaker_flush(speaker);
if (status != PV_SPEAKER_STATUS_SUCCESS) {
    // handle PvSpeaker flush error
}
```

5. Stop the speaker:

```c
pv_speaker_status_t status = pv_speaker_stop(speaker);
if (status != PV_SPEAKER_STATUS_SUCCESS) {
    // handle PvSpeaker stop error
}
```

6. Release resources used by PvSpeaker:

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
