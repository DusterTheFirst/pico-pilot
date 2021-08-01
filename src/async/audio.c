#include "async/audio.h"
#include "async/executor.h"
#include "globals.h"
#include "tone.h"
#include <pico/time.h>

static song_id_t current_song = AUDIO_SONG_NONE;
static song_id_t next_song = AUDIO_SONG_NONE;

static int64_t _audio_ready(alarm_id_t id, void *_) {
    AUDIO_FUTURE.ready = true;

    return 0;
}

static size_t current_note = 0;

static void audio_poll() {
    // The executor should never poll this future if it is not ready
    // but just in case, we will return early
    if (!AUDIO_FUTURE.ready) {
        return;
    }

    volatile const song_t *const song = &SONG_LIBRARY[current_song];

    if (current_note < song->length) {
        const note_t *const note = &song->notes[current_note];

        tonegen_start(&tonegen, note->f, note->s);
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
        tonegen_stop(&tonegen);

        if (next_song == AUDIO_SONG_NONE && song->loop) {
            // Do nothing, play the same song again
        } else {
            current_song = next_song;
            next_song = AUDIO_SONG_NONE;
        }
    }
}

void queue_song(song_id_t song) {
    next_song = song;
    sleep_ms(1);
}

future_t AUDIO_FUTURE = {
    .poll = audio_poll,
    .ready = true,
};
