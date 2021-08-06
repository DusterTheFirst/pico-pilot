#include "async/audio.h"
#include "async/executor.h"
#include "tone.h"
#include <pico/time.h>

static song_id_t current_song = AUDIO_SONG_NONE;
static song_id_t next_song = AUDIO_SONG_NONE;

tonegen_t audio_system_tonegen = NULL_TONEGEN;

static void audio_poll();

static future_t AUDIO_FUTURE = {
    .poll = audio_poll,
    .ready = true,
};

song_id_t audio_queue_song(song_id_t song) {
    song_id_t old_next = next_song;

    next_song = song;

    return old_next;
}

future_t *audio_system_init(uint8_t pin, PIO pio) {
    audio_system_tonegen = tonegen_init(pin, pio);

    return &AUDIO_FUTURE;
}

static int64_t _audio_ready(__attribute__((unused)) alarm_id_t id,
                            __attribute__((unused)) void *user_data) {
    AUDIO_FUTURE.ready = true;

    return 0;
}

static size_t current_note = 0;

static void audio_poll() {
    // Ensure the tone generator exists and has been initialized
    if (audio_system_tonegen.pin != NULL_TONEGEN.pin) {
        return;
    }

    // The executor should never poll this future if it is not ready
    // but just in case, we will return early
    if (!AUDIO_FUTURE.ready) {
        return;
    }

    volatile const song_t *const song = &SONG_LIBRARY[current_song];

    if (current_note < song->length) {
        const note_t *const note = &song->notes[current_note];

        tonegen_start(&audio_system_tonegen, note->f, note->s);
        AUDIO_FUTURE.ready = false;

        alarm_id_t alarm = add_alarm_in_ms(note->d,
                                           _audio_ready,
                                           NULL,
                                           true);

        if (alarm <= 0) {
            panic("ERROR: Unable to allocate timer for "
                  "audio(song = %d, note = %d)\n",
                  current_song, current_note);
        }

        current_note++;
    } else {
        current_note = 0;
        tonegen_stop(&audio_system_tonegen);

        if (next_song == AUDIO_SONG_NONE && song->loop) {
            // Do nothing, play the same song again
        } else {
            current_song = next_song;
            next_song = AUDIO_SONG_NONE;
        }
    }
}
