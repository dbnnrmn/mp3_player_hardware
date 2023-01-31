#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>

#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
#include <libavutil/opt.h>
#include <libavutil/dict.h>
#include <libavformat/avio.h>

#include <alsa/asoundlib.h>

#include "audiochank.h"


static void get_stream_info();
static AVCodecContext* get_codec_context(int streamIndex);
static audiochunk_queue_t queue;



static AVFormatContext *formatCtx = NULL;
static AVCodecContext *audioCodecCtx = NULL;
struct SwrContext *swrCtx = NULL;

int audioStreamIndex = -1;


void start_audio_player(const char *filename, int bufferSize)
{
    av_register_all();

    if (avformat_open_input(&formatCtx, filename, NULL, NULL) < 0) {
        printf("Error: invalid file.\n");
        exit(1);
    }
    
    get_stream_info();
    audioCodecCtx = get_codec_context(audioStreamIndex);
    show_audio_info();

}

static void get_stream_info()
{
    if (avformat_find_stream_info(formatCtx, NULL) < 0) {
        printf("Error: It wasn't possible to find stream information in the audio file.\n");
        exit(1);
    }
    
    for (int i = 0; i < formatCtx->nb_streams; i++) {
        if (formatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStreamIndex = i;
            break;
        }
    }
    
    if (audioStreamIndex == -1) {
        printf("Error: There is no audio stream in the specified file.\n");
        exit(1);
    }
}


static AVCodecContext* get_codec_context(int streamIndex)
{
    AVCodecContext *codecCtxOrig = formatCtx->streams[streamIndex]->codec;
    AVCodec *codec = avcodec_find_decoder(codecCtxOrig->codec_id);
    if (codec == NULL) {
        printf("Error: invalid audio codec\n");
        exit(1);  
    }
    else printf("codec is %d\n",codecCtxOrig->codec_id);
    
    AVCodecContext *codecCtx = avcodec_alloc_context3(codec);
    if (avcodec_copy_context(codecCtx, codecCtxOrig) < 0) {
        printf("Error: the audio codec context could not be copied.\n");
        exit(1);
    }
    avcodec_close(codecCtxOrig);
    
    if (avcodec_open2(codecCtx, codec, NULL) < 0) {
        printf("Error: the audio codec could not be opened.\n");
        exit(1);
    }
    
    return codecCtx;
};

void stop_audio_player(){
      avcodec_close(audioCodecCtx);
      avformat_close_input(&formatCtx);
}

void show_audio_info()
{
    printf("Filename: %s\n", formatCtx->filename);
    printf("Codec: %s\n", audioCodecCtx->codec_descriptor->long_name);
    if (audioCodecCtx->channels > 0 && audioCodecCtx->channels <= 2) {
        printf("Channels: %d (%s)\n", audioCodecCtx->channels, audioCodecCtx->channels == 1 ? "mono" : "stereo");
    } else {
        printf("Channels: %d\n", audioCodecCtx->channels);
    }
    printf("Sample rate: %d Hz\n", audioCodecCtx->sample_rate);
    printf("Duration: %lld ms\n", (long long) formatCtx->duration);
    
    AVDictionaryEntry *titleEntry = av_dict_get(formatCtx->metadata, "title", NULL, 0);
    if (titleEntry != NULL) {
        printf("Title: %s\n", titleEntry->value);
    }
    
    AVDictionaryEntry *artistEntry = av_dict_get(formatCtx->metadata, "artist", NULL, 0);
    if (artistEntry != NULL) {
        printf("Artist: %s\n", artistEntry->value);
    }
    
    AVDictionaryEntry *albumEntry = av_dict_get(formatCtx->metadata, "album", NULL, 0);
    if (albumEntry != NULL) {
        printf("Album: %s\n", albumEntry->value);
    }
    
    printf("\n\n");
}

static void _configureAudio()
{
    SwrContext swrCtx = swr_alloc();
    av_opt_set_int(swrCtx, "in_channel_count", audioCodecCtx->channels, 0);
    av_opt_set_int(swrCtx, "out_channel_count", audioCodecCtx->channels, 0);
    av_opt_set_int(swrCtx, "in_channel_layout", audioCodecCtx->channel_layout, 0);
    av_opt_set_int(swrCtx, "out_channel_layout", audioCodecCtx->channel_layout, 0);
    av_opt_set_int(swrCtx, "in_sample_rate", audioCodecCtx->sample_rate, 0);
    av_opt_set_int(swrCtx, "out_sample_rate", audioCodecCtx->sample_rate, 0);
    av_opt_set_sample_fmt(swrCtx, "in_sample_fmt", audioCodecCtx->sample_fmt, 0);
    av_opt_set_sample_fmt(swrCtx, "out_sample_fmt", AV_SAMPLE_FMT_S16,  0);
    swr_init(swrCtx);
    

}

static void* produce()
{
    AVPacket packet;
    AVFrame *audioFrame = av_frame_alloc();
    int finishedFrame;
    avcodec_decode_audio4(audioCodecCtx, audioFrame, &finishedFrame, &packet);

     audiochunk_t *audiochunk = malloc(sizeof(audiochunk_t));
     audiochunk->size = av_samples_get_buffer_size(NULL, audioCodecCtx->channels, audioFrame->nb_samples, AV_SAMPLE_FMT_S16, 1);
     audiochunk->data = malloc(audiochunk->size);
     swr_convert(swrCtx, &audiochunk->data, audiochunk->size, (const uint8_t **) audioFrame->data, audioFrame->nb_samples);
     insertAudioChunk(audiochunk, queue);
     av_frame_unref(audioFrame);




}