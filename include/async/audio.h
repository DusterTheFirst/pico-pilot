#pragma once

#include "executor.h"

extern future_t AUDIO_FUTURE;

typedef enum SongID {
    AUDIO_SONG_NONE,
    AUDIO_SONG_A4,
    AUDIO_STARTUP_CHIRP,
    AUDIO_SONG_MEGALOVANIA,
} song_id_t;

typedef struct Note {
    double f;   // Frequency
    uint32_t s; // Sustain
    uint32_t d; // Delay
} note_t;

typedef struct Song {
    note_t *const notes;
    size_t length;
    bool loop;
} song_t;

void queue_song(song_id_t song);

extern const song_t SONG_LIBRARY[];
