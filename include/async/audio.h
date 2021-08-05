#pragma once

#include "executor.h"
#include "tone.h"
#include <hardware/pio.h>

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

// Add a song to the audio subsystem queue.
// Returns the ID of the song in the queue if there was one
song_id_t audio_queue_song(song_id_t song);
// Initialize the audio subsystem
future_t *audio_system_init(uint8_t pin, PIO pio);

extern const song_t SONG_LIBRARY[];
extern tonegen_t audio_system_tonegen;
