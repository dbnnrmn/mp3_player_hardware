#include <alsa/asoundlib.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE

typedef struct async_private_data {
        signed short* first_buffer;  
        signed short* second_buffer;
        signed short* current_buffer;
        pthread_mutex_t buf_mut;
};




typedef struct alsa_layer
{
    snd_pcm_t *handle;
    unsigned int rate;                       /* stream rate */
    unsigned int channels;                   /* count of channels */
    unsigned int buffer_time;               /* ring buffer length in us */
    unsigned int period_time ;               /* period time in us */
    snd_pcm_format_t format;
    async_private_data handler_context;
};

void alsa_init(alsa_layer* layer){

    snd_pcm_open(layer->handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

    snd_pcm_hw_params_t *hw_params;
    snd_pcm_hw_params_alloca(&hw_params);
    snd_pcm_hw_params_any(layer->handle, hw_params);
    snd_pcm_hw_params_set_access(layer->handle, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
    snd_pcm_hw_params_set_format(layer->handle, hw_params, layer->format);
    snd_pcm_hw_params_set_channels(layer->handle, hw_params, layer->channels);
    snd_pcm_hw_params_set_rate(layer->handle, hw_params, layer->rate, 0);
    snd_pcm_hw_params_set_periods(layer->handle, hw_params, 10, 0);
    snd_pcm_hw_params_set_period_time(layer->handle, hw_params, layer->period_time, 0); // 0.1 seconds

    snd_pcm_hw_params(layer->handle, hw_params);

}

static void async_callback(snd_async_handler_t *ahandler)
{
        static int frame_count=0;
        snd_pcm_t *handle = snd_async_handler_get_pcm(ahandler);
        struct async_private_data *data = snd_async_handler_get_callback_private(ahandler);

        if(count&1) current_buffer = data->first_buffer;
        else current_buffer=data->second_buffer;

        if(current_buffer==NULL) 
        {
            printf("Current alsa buffer is empty!");
            exit(EXIT_FAILURE);
        }
        snd_pcm_sframes_t avail;
        int err;
        
        avail = snd_pcm_avail_update(handle);
        while (avail >= period_size) {
                err = snd_pcm_writei(handle, current_buffer, period_size);
                if (err < 0) {
                        printf("Write error: %s\n", snd_strerror(err));
                        exit(EXIT_FAILURE);
                }
                if (err != period_size) {
                        printf("Write error: written %i expected %li\n", err, period_size);
                        exit(EXIT_FAILURE);
                }
                avail = snd_pcm_avail_update(handle);
        }

        frame_count++;
}


void alsa_start(alsa_layer* layer){

       snd_async_handler_t *ahandler;
       err = snd_async_add_pcm_handler(&ahandler, layer->pcm, async_callback, &(layer->handler_context));
        if (err < 0) {
                printf("Unable to register async handler\n");
                exit(EXIT_FAILURE);
        }
       if (snd_pcm_state(handle) == SND_PCM_STATE_PREPARED) {
                err = snd_pcm_start(layer->handle);
                if (err < 0) {
                        printf("Start error: %s\n", snd_strerror(err));
                        exit(EXIT_FAILURE);
                }
        }
 
}