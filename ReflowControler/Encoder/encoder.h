/*
 * encoder.h
 *
 * Created: 24.03.2014 10:54:46
 *  Author: Henry
 */ 
#ifndef __ENCODER_H__
#define __ENCODER_H__

#if __cplusplus
extern "C" {
#endif

typedef enum { Enc_None, Enc_Left, Enc_Right, Enc_Click, Enc_Escape } Encoder_Event_t;

typedef void (*ENCODER_EVENT)(Encoder_Event_t);

extern void encoder_init(void);
extern void encoder_register_event( ENCODER_EVENT );
extern void encoder_cyclic(void);

#if __cplusplus
}
#endif

#endif