/*
**  (C) Remo Dentato (rdentato@gmail.com)
**  UMF is distributed under the terms of the MIT License
**  as detailed in the 'LICENSE' file.
*/

#ifndef UMF_H
#define UMF_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>


typedef int16_t (*mf_fn_error   ) (int16_t err, char *msg);
typedef int16_t (*mf_fn_header  ) (int16_t type, int16_t ntracks, int16_t division);
typedef int16_t (*mf_fn_track   ) (int16_t eot,  int16_t tracknum, uint32_t tracklen);
typedef int16_t (*mf_fn_midi_evt) (uint32_t delta, int16_t type, int16_t chan,
                                                   int16_t data1, int16_t data2);
typedef int16_t (*mf_fn_sys_evt ) (uint32_t delta, int16_t type, int16_t aux,
                                                   int32_t len,  uint8_t *data);


typedef struct {
  FILE            *file        ;
  uint8_t         *chrbuf      ;
  uint32_t         chrbuf_sz   ;
  mf_fn_error      on_error    ;
  mf_fn_header     on_header   ;
  mf_fn_track      on_track    ;
  mf_fn_midi_evt   on_midi_evt ;
  mf_fn_sys_evt    on_sys_evt  ;
  void            *aux;
} mf_reader;


int16_t mf_scan(mf_reader *mfile);

void mf_reader_close(mf_reader *mr);

int16_t mf_read( char           *fname       ,
                 mf_fn_error     fn_error    ,
                 mf_fn_header    fn_header   ,
                 mf_fn_track     fn_track    ,
                 mf_fn_midi_evt  fn_midi_evt ,
                 mf_fn_sys_evt   fn_sys_evt
               );

#define mf_cc_bank_select                     0x00
#define mf_cc_modulation_wheel                0x01
#define mf_cc_breath_controller               0x02
#define mf_cc_foot_controller                 0x04
#define mf_cc_portamento_time                 0x05
#define mf_cc_data_entry                      0x06
#define mf_cc_channel_volume                  0x07
#define mf_cc_balance                         0x08
#define mf_cc_pan                             0x0a
#define mf_cc_expression_controller           0x0b
#define mf_cc_effect_control1                 0x0c
#define mf_cc_effect_control2                 0x0d
#define mf_cc_bank_select_lsb                 0x20
#define mf_cc_modulation_wheel_lsb            0x21
#define mf_cc_breath_controller_lsb           0x22
#define mf_cc_foot_controller_lsb             0x24
#define mf_cc_portamento_lsb                  0x25
#define mf_cc_data_entry_lsb                  0x26
#define mf_cc_channel_volume_lsb              0x27
#define mf_cc_balance_lsb                     0x28
#define mf_cc_pan_lsb                         0x2a
#define mf_cc_expression_controller_lsb       0x2b
#define mf_cc_effect_control1_lsb             0x2c
#define mf_cc_effect_control2_lsb             0x2d
#define mf_cc_damper_pedal                    0x40
#define mf_cc_portamento                      0x41
#define mf_cc_sostenuto                       0x42
#define mf_cc_soft_pedal                      0x43
#define mf_cc_legato                          0x44
#define mf_cc_hold_2                          0x45
#define mf_cc_sound_variation                 0x46
#define mf_cc_timbre_intensity                0x47
#define mf_cc_harmonic_intensity              0x47
#define mf_cc_release_time                    0x48
#define mf_cc_attack_time                     0x49
#define mf_cc_brightness                      0x4a
#define mf_cc_decay_time                      0x4b
#define mf_cc_vibrato_rate                    0x4c
#define mf_cc_vibrato_depth                   0x4d
#define mf_cc_vibrato_delay                   0x4e
#define mf_cc_portamento_control              0x54
#define mf_cc_reverb_send                     0x5b
#define mf_cc_tremolo_depth                   0x5c
#define mf_cc_chorus_send                     0x5d
#define mf_cc_detune                          0x5e
#define mf_cc_phaser_depth                    0x5f
#define mf_cc_data_increment                  0x60
#define mf_cc_data_decrement                  0x61
#define mf_cc_non_registered_number_lsb       0x62
#define mf_cc_non_registered_number           0x63
#define mf_cc_registered_number_lsb           0x64
#define mf_cc_registered_number               0x65
#define mf_cc_all_sound_off                   0x78
#define mf_cc_reset_all_controllers           0x79
#define mf_cc_local_control                   0x7a
#define mf_cc_all_notes_off                   0x7b
#define mf_cc_omni_mode_off                   0x7c
#define mf_cc_omni_mode_on                    0x7d
#define mf_cc_mono_mode_on                    0x7e
#define mf_cc_poly_mode_off                   0x7e
#define mf_cc_poly_mode_on                    0x7f
#define mf_cc_mono_mode_off                   0x7f

/* Registered parameter number */
#define mf_cc_pitch_bend_sensitivity          0x80
#define mf_cc_channel_fine_tuning             0x81
#define mf_cc_channel_coarse_tuning           0x82
#define mf_cc_tuning_program                  0x83
#define mf_cc_tuning_bank                     0x84
#define mf_cc_modulation_depth_range          0x85
#define mf_cc_rpn_reset                       0xFF

/* RPN or N-RPN (http://www.philrees.co.uk/nrpnq.htm)*/

#define mf_cc_nrpn(h,l) ((((h) &0x7F)<<8) | ((l)&0x7F) | 0x8000)
#define mf_cc_rpn(h,l)  ((((h) &0x7F)<<8) | ((l)&0x7F) | 0x0080)

#define mf_st_note_off                0x80
#define mf_st_note_on                 0x90
#define mf_st_key_pressure            0xA0
#define mf_st_control_change          0xB0
#define mf_st_program_change          0xC0
#define mf_st_channel_pressure        0xD0
#define mf_st_pitch_bend              0xE0
#define mf_st_system_exclusive        0xF0
#define mf_st_system_continue         0xF7
#define mf_st_meta_event              0xFF

#define mf_me_sequence_number         0x00

#define mf_me_text                    0x01
#define mf_me_copyright_notice        0x02
#define mf_me_sequence_name           0x03
#define mf_me_track_name              0x03
#define mf_me_instrument_name         0x04
#define mf_me_lyrics                  0x05
#define mf_me_marker                  0x06
#define mf_me_cue_point               0x07
#define mf_me_device_name             0x08
#define mf_me_program_name            0x09

#define mf_me_end_of_track            0x2f
#define mf_me_set_tempo               0x51
#define mf_me_smpte_offset            0x54
#define mf_me_time_signature          0x58
#define mf_me_key_signature           0x59
#define mf_me_sequencer_specific      0x7F
#define mf_me_channel_prefix          0x20
#define mf_me_port_prefix             0x21



/********************************************/
#define mf_type_file 1


typedef struct {
  uint16_t  type;
  FILE     *file;
  uint32_t  len_pos;    /* where to write (in the file) the track len */
  uint32_t  trk_len;    /* Total track length */
  int16_t   trk_cnt;    /* How many tracks? */
  int16_t   division;
  int16_t   trk_in;     /* 0: before track 1: in track */
  int16_t   chan;       /* current channel  (0-15) */
} mf_writer;

mf_writer *mf_new (char *fname, int16_t division);
int16_t mf_close (mf_writer *mw);

int16_t mf_track_start (mf_writer *mw);
int16_t mf_track_end   (mf_writer *mw);

int16_t mf_midi_evt (mf_writer *mw, uint32_t delta, int16_t type, int16_t chan,
                                                  int16_t data1, int16_t data2);
int16_t mf_sys_evt  (mf_writer *mw, uint32_t delta, int16_t type, int16_t aux,
                                                  int32_t len, uint8_t *data);

int16_t mf_text_evt (mf_writer *mw, uint32_t delta, int16_t type, char *txt);


#define mf_note_off(m,d,c,p)            mf_midi_evt(m, d, mf_st_note_off, c, p, 0)
#define mf_note_on(m,d,c,p,v)           mf_midi_evt(m, d, mf_st_note_on , c, p, v)

#define mf_key_pressure(m,d,c,p,r)      mf_midi_evt(m, d, mf_st_key_pressure, c, p, r)
#define mf_channel_pressure(m,d,c,r)    mf_midi_evt(m, d, mf_st_channel_pressure, c, r, 0)

#define mf_program_change(m,d,c,i)      mf_midi_evt(m, d, mf_st_program_change, c, i, 0)

#define mf_control_change(m,d,c,ctr,v)  mf_midi_evt(m, d, mf_st_control_change, c, ctr, v)


int16_t mf_pitch_bend(mf_writer *mw, uint32_t delta, uint8_t chan, int16_t bend);

int16_t mf_set_tempo(mf_writer *mw, uint32_t delta, int32_t tempo);

#define mf_set_bpm(m,d,t) mf_set_tempo(m,d, (60000000L / (int32_t)(t)))

int16_t mf_set_keysig(mf_writer *mw, uint32_t delta, int16_t accid, int16_t mi);

/*

void mf_sequence_number(uint32_t delta, int16_t seqnum);

void mf_time_signature(uint32_t delta, char num, char den,
                                                    char clks, char q32nd);
void mf_sysex(uint32_t delta, int16_t type, int32_t len, char *data);
void mf_sequencer_specific(uint32_t delta, int32_t len, char *data);

*/

#define mf_text(m,d,t)              mf_text_evt(m, d, mf_me_text             ,t)
#define mf_copyright_notice(m,d,t)  mf_text_evt(m, d, mf_me_copyright_notice ,t)
#define mf_sequence_name(m,d,t)     mf_text_evt(m, d, mf_me_sequence_name    ,t)
#define mf_track_name(m,d,t)        mf_text_evt(m, d, mf_me_track_name       ,t)
#define mf_instrument_name(m,d,t)   mf_text_evt(m, d, mf_me_instrument_name  ,t)
#define mf_lyric(m,d,t)             mf_text_evt(m, d, mf_me_lyric            ,t)
#define mf_marker(m,d,t)            mf_text_evt(m, d, mf_me_marker           ,t)
#define mf_cue_point(m,d,t)         mf_text_evt(m, d, mf_me_cue_point        ,t)
#define mf_device_name(m,d,t)       mf_text_evt(m, d, mf_me_device_name      ,t)
#define mf_program_name(m,d,t)      mf_text_evt(m, d, mf_me_program_name     ,t)

int16_t mf_numparms(int16_t s);

/*****************************/

#define mf_type_seq 2
#define mf_type_msq 3

#define MF_MAX_TRACKS 32
#define MF_NO_TICK 0xFFFFFFFE
#define MF_MAX_SAV 10
#define MF_DIVISION 960
#define MF_NOVAL -1
#define MF_UNSORTED       0
#define MF_SORTED_BYTRACK 1
#define MF_SORTED_BYTICK  2
#define MF_NO_EVENT 0xFFFFFFFE

typedef struct {
  uint16_t type;
  uint16_t flags;
  
  uint8_t  *buf;  uint32_t buf_cnt;  uint32_t buf_max;
  uint32_t *evt;  uint32_t evt_cnt;  uint32_t evt_max;
  
  char    *fname;
  int16_t  division;
  int16_t  curtrack;
  uint32_t curevt;
  uint32_t savtick[MF_MAX_SAV];
  uint32_t curtick[MF_MAX_TRACKS];
  uint32_t curdur[MF_MAX_TRACKS];
  uint8_t  curchan[MF_MAX_TRACKS];  
  uint8_t  curvel[MF_MAX_TRACKS];
  uint8_t  curnote[MF_MAX_TRACKS];
  uint16_t cursav;

} mf_seq;  

mf_seq *mf_seq_new (char *fname, uint16_t division);
int16_t mf_seq_close(mf_seq *ms);
int16_t mf_seq_set_track(mf_seq *ms, int16_t track);
int16_t mf_seq_get_track(mf_seq *ms);
int16_t mf_seq_evt(mf_seq *ms, uint32_t tick, uint16_t type, uint16_t chan, uint16_t data1, uint16_t data2);
int16_t mf_seq_sys(mf_seq *ms, uint32_t tick, uint16_t type, uint16_t aux, int32_t len, uint8_t *data);

#define mf_seq_txt_evt(ms, tick, type, txt)   mf_seq_sys(ms, tick, mf_st_meta_event, (type) & 0x0F, -1, (uint8_t *)(txt))
#define mf_seq_text(ms,d,t)                   mf_seq_txt_evt(m, d, mf_me_text             ,t)
#define mf_seq_copyright_notice(m,d,t)        mf_seq_txt_evt(m, d, mf_me_copyright_notice ,t)
#define mf_seq_sequence_name(m,d,t)           mf_seq_txt_evt(m, d, mf_me_sequence_name    ,t)
#define mf_seq_track_name(m,d,t)              mf_seq_txt_evt(m, d, mf_me_track_name       ,t)
#define mf_seq_instrument_name(m,d,t)         mf_seq_txt_evt(m, d, mf_me_instrument_name  ,t)
#define mf_seq_lyrics(m,d,t)                  mf_seq_txt_evt(m, d, mf_me_lyrics           ,t)
#define mf_seq_marker(m,d,t)                  mf_seq_txt_evt(m, d, mf_me_marker           ,t)
#define mf_seq_cue_point(m,d,t)               mf_seq_txt_evt(m, d, mf_me_cue_point        ,t)
#define mf_seq_device_name(m,d,t)             mf_seq_txt_evt(m, d, mf_me_device_name      ,t)
#define mf_seq_program_name(m,d,t)            mf_seq_txt_evt(m, d, mf_me_program_name     ,t)

#define mf_seq_note_off(m,d,c,p)            mf_seq_evt(m, d, mf_st_note_off         , c, p, 0)
#define mf_seq_note_on(m,d,c,p,v)           mf_seq_evt(m, d, mf_st_note_on          , c, p, v)

#define mf_seq_key_pressure(m,d,c,p,r)      mf_seq_evt(m, d, mf_st_key_pressure     , c, p, r)
#define mf_seq_channel_pressure(m,d,c,r)    mf_seq_evt(m, d, mf_st_channel_pressure , c, r, 0)

#define mf_seq_program_change(m,d,c,i)      mf_seq_evt(m, d, mf_st_program_change   , c, i, 0)

#define mf_seq_control_change(m,d,c,ctr,v)  mf_seq_evt(m, d, mf_st_control_change   , c, ctr, v)

int16_t mf_seq_set_keysig(mf_seq *ms, uint32_t tick, int16_t acc, int16_t mi);
int16_t mf_seq_set_tempo(mf_seq *ms, uint32_t tick, int32_t tempo);
#define mf_seq_set_bpm(m,d,t) mf_seq_set_tempo(m,d, (60000000L / (int32_t)(t)))
int16_t mf_seq_pitch_bend(mf_seq *ms, uint32_t tick, uint8_t chan, int16_t bend);
 
int16_t mf_seq_note(mf_seq *ms, uint16_t pitch, uint32_t dur, uint16_t vel);
int16_t mf_seq_rest(mf_seq *ms, uint32_t dur);
int16_t mf_seq_track(mf_seq *ms, uint16_t track);
int16_t mf_seq_channel(mf_seq *ms, uint16_t chn, uint16_t track);

uint32_t mf_seq_set_mark(mf_seq *ms, uint32_t mrk, uint32_t tick);
uint32_t mf_seq_get_mark(mf_seq *ms, uint32_t mrk);
uint32_t mf_seq_tick(mf_seq *ms, uint32_t mrk, uint32_t tick);

#define mf_seq_sorted(m) ((m)->flags & (MF_SORTED_BYTICK | MF_SORTED_BYTRACK))
int16_t mf_seq_bytrack(mf_seq *ms);
int16_t mf_seq_bytick(mf_seq *ms);

uint8_t mf_pitch_str(char *s);

/* ****************************** */


#define mf_whole_n(m)      (((m)->division) * 4)
#define mf_half_n(m)       (((m)->division) * 2)
#define mf_quarter_n(m)    (((m)->division))
#define mf_eigth_n(m)      (((m)->division) / 2)
#define mf_sixteenth_n(m)  (((m)->division) / 4)

#define mf_dot_n(d)         (((d)*3)/2)
#define mf_ddot_n(d)        (((d)*7)/8)
#define mf_tuplet_n(n,m,d)  (((d)*(m))/(n))

#define mf_expand(x) x
#define mf_arg0(_x0,...)                                     _x0
#define mf_arg1(_x0,_x1,...)                                 _x1
#define mf_arg2(_x0,_x1,_x2,...)                             _x2
#define mf_arg3(_x0,_x1,_x2,_x3,...)                         _x3
#define mf_arg4(_x0,_x1,_x2,_x3,_x4,...)                     _x4
#define mf_arg5(_x0,_x1,_x2,_x3,_x4,_x5,...)                 _x5
#define mf_arg6(_x0,_x1,_x2,_x3,_x4,_x5,_x6,...)             _x6
#define mf_arg7(_x0,_x1,_x2,_x3,_x4,_x5,_x6,_x7,...)         _x7
#define mf_arg8(_x0,_x1,_x2,_x3,_x4,_x5,_x6,_x7,_x8,...)     _x8
#define mf_arg9(_x0,_x1,_x2,_x3,_x4,_x5,_x6,_x7,_x8,_x9,...) _x9

#define ms_new(...)  mf_seq_new(mf_arg0(__VA_ARGS__,NULL),\
                                mf_arg1(__VA_ARGS__, MF_DIVISION, MF_DIVISION))

#define ms_close(m)  mf_seq_close(m)

#define ms_note(...)  mf_seq_note(mf_arg0(__VA_ARGS__,NULL),\
                   /* pitch   */  mf_arg1(__VA_ARGS__, MF_NOVAL, MF_NOVAL), \
                   /* duration*/  mf_arg2(__VA_ARGS__, MF_NOVAL, MF_NOVAL, MF_NOVAL), \
                   /* velocity*/  mf_arg3(__VA_ARGS__, MF_NOVAL, MF_NOVAL, MF_NOVAL, MF_NOVAL))
                      

#define ms_rest(...)   mf_seq_rest(mf_arg0(__VA_ARGS__,NULL),\
                   /* duration*/   mf_arg1(__VA_ARGS__, MF_NOVAL, MF_NOVAL)) 
                         

#define ms_track(...) mf_seq_track(mf_arg0(__VA_ARGS__,NULL) , \
                                   mf_arg1(__VA_ARGS__, MF_MAX_TRACKS, MF_MAX_TRACKS))

#define ms_channel(...) mf_seq_channel(mf_arg0(__VA_ARGS__,NULL) , \
                                       mf_arg1(__VA_ARGS__, MF_NOVAL, MF_NOVAL), \
                                       mf_arg3(__VA_ARGS__, MF_MAX_TRACKS, MF_MAX_TRACKS, MF_MAX_TRACKS))

#define mf_markA   0xFFFFFFF0
#define mf_markB   0xFFFFFFF1
#define mf_markC   0xFFFFFFF2
#define mf_markD   0xFFFFFFF3
#define mf_markE   0xFFFFFFF4
#define mf_markF   0xFFFFFFF5
#define mf_markG   0xFFFFFFF6
#define mf_markH   0xFFFFFFF7
#define mf_markI   0xFFFFFFF8
#define mf_markJ   0xFFFFFFF9
#define MF_NO_MARK 0xFFFFFFFE

#define ms_setmark(...) mf_seq_set_mark(mf_arg0(__VA_ARGS__,NULL) , \
                                    mf_arg1(__VA_ARGS__, mf_markA, mf_markA), \
                                    mf_arg2(__VA_ARGS__, MF_NO_TICK, MF_NO_TICK, MF_NO_TICK))

#define ms_getmark(...) mf_seq_get_mark(mf_arg0(__VA_ARGS__,NULL) , \
                                    mf_arg1(__VA_ARGS__, MF_NO_MARK, MF_NO_MARK))

#define ms_tick(...) mf_seq_tick(mf_arg0(__VA_ARGS__,NULL) , \
                                 mf_arg1(__VA_ARGS__, MF_NO_TICK, MF_NO_TICK))

#define mf_key_C          0,0
#define mf_key_Cs         7,0 
#define mf_key_Db        -5,0
#define mf_key_D          2,0
#define mf_key_Ds        -3,0
#define mf_key_Eb        -3,0
#define mf_key_E          4,0
#define mf_key_F         -1,0
#define mf_key_Fs         6,0
#define mf_key_Gb        -6,0
#define mf_key_G          1,0
#define mf_key_Gs        -4,0
#define mf_key_Ab        -4,0
#define mf_key_A          3,0
#define mf_key_As        -2,0
#define mf_key_Bb        -2,0  
#define mf_key_B          5,0

#define mf_key_C_min     -3,1
#define mf_key_Cs_min     4,1
#define mf_key_Db_min 
#define mf_key_D_min     -1,1
#define mf_key_Ds_min     6,1
#define mf_key_Eb_min    -6,1
#define mf_key_E_min      1,1
#define mf_key_F_min     -4,1
#define mf_key_Fs_min     3,1
#define mf_key_Gb_min 
#define mf_key_G_min     -2,1
#define mf_key_Gs_min     5,1
#define mf_key_Ab_min 
#define mf_key_A_min      0,1
#define mf_key_As_min     7,1
#define mf_key_Bb_min    -5,1
#define mf_key_B_min      2,1



/* Ab        -4,0 */
/* A          3,0 */
/* As        -2,0 */
/* Bb        -2,0 */  
/* B          5,0 */
/* Bb         5,0 */
/* Cd         5,0 */
/* C          0,0 */
/* Cs         7,0 */ 
/* Db        -5,0 */
/* D          2,0 */
/* Ds        -3,0 */
/* Eb        -3,0 */
/* E          4,0 */
/* Es        -1,0 */
/* Fb         4,0 */
/* F         -1,0 */
/* Fs         6,0 */
/* Gb        -6,0 */
/* G          1,0 */
/* Gs        -4,0 */

#endif
