//
//  buffer.h
//
#ifndef __BUFFER_H__
#define __BUFFER_H__

#include "buffer.h"

typedef struct {
    int keycode;
    int isPress;
} OSD_EVENT;

#define MAX_EVENT 10

void initEvent(void);
int writeEvent( OSD_EVENT data);
int readEvent( OSD_EVENT *data);

#endif
