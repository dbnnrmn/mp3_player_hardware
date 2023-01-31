#include "stdio.h"
#include <alsa/asoundlib.h>
#include "math.h"
#include "player/player.h"



int main(){


start_audio_player("/media/roman/F8E0A672E0A63732/my/projects/mp3_test_board/software/software/src/test.wav",100);
stop_audio_player();

printf("hello");
return 0;


}