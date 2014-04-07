/*
 * ParamTask.h
 *
 * Created: 30.03.2014 19:10:48
 *  Author: Henry
 */ 


#ifndef PARAMTASK_H_
#define PARAMTASK_H_

#if __cplusplus
extern "C" {
#endif

extern void param_task(MESSAGE_PUMP pMsgPump, uint8_t ParamNo);
extern void param_bank_task(MESSAGE_PUMP pMsgPump);
extern void param_act_thrsh(MESSAGE_PUMP pMsgPump);

#if __cplusplus
};
#endif

#endif /* PARAMTASK_H_ */