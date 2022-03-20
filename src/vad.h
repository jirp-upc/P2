#ifndef _VAD_H
#define _VAD_H
#include <stdio.h>

/* TODO: add the needed states */
typedef enum {ST_UNDEF=0, ST_SILENCE, ST_VOICE, ST_INIT} VAD_STATE;

/* Return a string label associated to each state */
const char *state2str(VAD_STATE st);

/* TODO: add the variables needed to control the VAD 
   (counts, thresholds, etc.) */

typedef struct {
  VAD_STATE state;
  float sampling_rate;
  unsigned int frame_length;
  float last_feature; /* for debuggin purposes */
  float p1; // == k1
  float p0; // == k0
  float alpha1;
  float init_time; // = 200*1e-3; //Tiempo de margen de silencio inicial
  unsigned int count; //contador de estados maybe consecutivos 
  float time_elapsed; //tiempo que ha pasado (crono)
  float max_time_unknown; //tiempo máximo de maybe
  VAD_STATE last_state_known; //último estado determinado
  unsigned int frames_silence_counted;
  unsigned int sil_calc;  //indicador de si ya hay umbral calculado
  float first_feature; //backup en caso de que no haya ni una muestra válida para calcular umbral

} VAD_DATA;

/* Call this function before using VAD: 
   It should return allocated and initialized values of vad_data

   sampling_rate: ... the sampling rate */
VAD_DATA *vad_open(float sampling_rate, float alpha1);

/* vad works frame by frame.
   This function returns the frame size so that the program knows how
   many samples have to be provided */
unsigned int vad_frame_size(VAD_DATA *);

/* Main function. For each 'time', compute the new state 
   It returns:
    ST_UNDEF   (0) : undefined; it needs more frames to take decission
    ST_SILENCE (1) : silence
    ST_VOICE   (2) : voice

    x: input frame
       It is assumed the length is frame_length */
VAD_STATE vad(VAD_DATA *vad_data, float *x);

/* Free memory
   Returns the state of the last (undecided) states. */
VAD_STATE vad_close(VAD_DATA *vad_data);

/* Print actual state of vad, for debug purposes */
void vad_show_state(const VAD_DATA *, FILE *);

#endif
