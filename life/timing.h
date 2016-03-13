#ifndef TIMING_H_
#define TIMING_H_

extern volatile unsigned int timing_tick;
extern volatile unsigned int timing_action_tick;
extern int timing_action_fps;

void timing_set_action_fps(int fps);
void timing_init(int fps);

#endif

