
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "pav_analysis.h"
#include "vad.h"

const float FRAME_TIME = 10.0F; //in ms. 
const unsigned int ZCR_MINIMUM_NOISE = 5;
/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
    "UNDEF", "S", "V", "INIT"};

const char *state2str(VAD_STATE st)
{
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct
{
  float zcr;
  float p;
  float am;

} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N)
{
  /*
   * Input: x[i] : i=0 .... N-1 
   * Ouput: computed features
   */
  /* 
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1 
   */
  Features feat;
  //feat.zcr = feat.p = feat.am = (float) rand()/RAND_MAX;
  feat.zcr = compute_zcr(x, N, 16000);
  feat.am = compute_am(x, N);
  feat.p = compute_power(x, N);
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */

VAD_DATA *vad_open(float rate, float alpha1)
{
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  vad_data->alpha1 = alpha1;
  vad_data->init_time = 200 * 1e-3; //init = 200
  vad_data->count = 0;
  vad_data->max_time_unknown = 100 * 1e-3;
  vad_data->time_elapsed = 0;
  vad_data->frames_silence_counted = 0;
  vad_data->sil_calc = 0;
  vad_data->p1 = 0;
  vad_data->alpha1 = alpha1;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data)
{
  /* 
   * TODO: decide what to do with the last undecided frames
   */
  VAD_STATE state = vad_data->state;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data)
{
  return vad_data->frame_length;
}

/* 
 * TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x){

  /* 
   * TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */

  Features f = compute_features(x, vad_data->frame_length);
  vad_data->last_feature = f.p; /* save feature, in case you want to show */
  if(!vad_data->sil_calc && vad_data->time_elapsed > vad_data->init_time){
     if (vad_data->frames_silence_counted > 0){
          vad_data->p1 /= vad_data->frames_silence_counted;
        }
        else{
          vad_data->p1 = vad_data->first_feature;
        }
        vad_data->p1 += vad_data->alpha1;
        vad_data->sil_calc = 1;
  }
  switch (vad_data->state){

  case ST_INIT:
    vad_data->state = ST_SILENCE; //Primera iteración, vamos a silencio
    if (f.zcr >= ZCR_MINIMUM_NOISE){
      vad_data->p1 = f.p;
      vad_data->frames_silence_counted++;
      vad_data->first_feature = f.p;
      //vad_data->p1 = f.p+vad_data->alpha1;
    }
      break;

    case ST_SILENCE:
      if (vad_data->time_elapsed > vad_data->init_time){ //Si supera margen inicial,
        if (f.p > vad_data->p1){ //Si potencia > 0.95, ¿es voz?
          //vad_data->state = ST_UNDEF;
          vad_data->state = ST_VOICE;
          vad_data->last_state_known = ST_SILENCE;
        }
      }
      else if (!vad_data->sil_calc){
        if (f.zcr >= ZCR_MINIMUM_NOISE){
          vad_data->frames_silence_counted++;
          vad_data->p1 += f.p;
        }
      }
      else{
       //
      }
      break;

    case ST_VOICE:
      if (f.p < vad_data->p1){ //Si potencia < 0.01, ¿es silencio?
        //vad_data->state = ST_UNDEF;
        vad_data->state = ST_SILENCE;
        vad_data->last_state_known = ST_VOICE;
      }
      break;

    case ST_UNDEF:
      vad_data->count++;
      if (vad_data->count * FRAME_TIME > vad_data->max_time_unknown){
        if (f.p <= vad_data->p1) vad_data->state = ST_VOICE;
        else vad_data->state = ST_SILENCE;
        vad_data->last_state_known = ST_UNDEF; //Para debug, no afecta funcionamiento esta asignación.
        vad_data->count = 0;
      }
      break;
    }
    
  //printf("Umbral: %f || Valor trama: %f || Decisión: %d || Instant: %f\n", vad_data->p1, f.p, vad_data->state, vad_data->time_elapsed);
  //if(vad_data->frames_silence_counted)
  vad_data->time_elapsed += FRAME_TIME * 1e-3; //Actualización crono
  if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE)
    return vad_data->state;
  else
    return ST_UNDEF;
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out)
{
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}


