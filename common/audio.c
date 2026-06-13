#define MINIAUDIO_IMPLEMENTATION
#include "../libs/miniaudio.h"
#include "audio.h"
#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <libgen.h>
#endif

static ma_engine engine;
static int engine_initialized = 0;

// Helper to resolve absolute path to assets directory
static void get_absolute_path(const char *relative_path, char *out_path, size_t max_len) {
#ifdef _WIN32
    char path[MAX_PATH];
    GetModuleFileNameA(NULL, path, MAX_PATH);
    char *last_slash = strrchr(path, '\\');
    if (last_slash) *last_slash = '\0';
    snprintf(out_path, max_len, "%s\\%s", path, relative_path);
#else
    char path[1024];
    ssize_t count = readlink("/proc/self/exe", path, sizeof(path) - 1);
    if (count != -1) {
        path[count] = '\0';
        char *dir = dirname(path);
        snprintf(out_path, max_len, "%s/%s", dir, relative_path);
    } else {
        snprintf(out_path, max_len, "%s", relative_path);
    }
#endif
}

void init_audio(void) {
    if (!engine_initialized) {
        if (ma_engine_init(NULL, &engine) == MA_SUCCESS) {
            engine_initialized = 1;
        } else {
            printf("Failed to initialize audio engine.\n");
        }
    }
}

void uninit_audio(void) {
    if (engine_initialized) {
        ma_engine_uninit(&engine);
        engine_initialized = 0;
    }
}

void play_sound(const char *filepath) {
    if (engine_initialized) {
        char abs_path[1024];
        get_absolute_path(filepath, abs_path, sizeof(abs_path));
        ma_engine_play_sound(&engine, abs_path, NULL);
    }
}
