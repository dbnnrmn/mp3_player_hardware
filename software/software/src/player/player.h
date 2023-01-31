#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

void start_audio_player(const char *filename, int buffer_size);

void stop_audio_player();

void pauseAudioPlayer();

void playAudioPlayer();


#endif