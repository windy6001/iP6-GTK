//
//  buffer.c
//
#include <stdio.h>
#include "types.h"
#include "buffer.h"

static int read;
static int write;
static int data_num;
static int size;
static int bottom;
static OSD_EVENT osd_event[MAX_EVENT];

// ******** 初期化 *****************
void initEvent(void)
{
    read= write = data_num = 0;
    size= MAX_EVENT;
    bottom = MAX_EVENT-1;
}

// ******** 書き込み *****************
// In:  data: write data
// Out: TRUE: success  FALSE: failed
int writeEvent( OSD_EVENT data)
{
    int ret= FALSE;
    if( data_num < size) {
        osd_event[write].keycode = data.keycode;
        osd_event[write].isPress = data.isPress;
        write++;
        if( write > bottom) {
            write =0;
        }
        ret = TRUE;
        data_num++;
    }
    return ret;
}

// ******** 読み込み *****************
// In:  data: read data
// Out: TRUE: success  FALSE: failed
int readEvent( OSD_EVENT *data)
{
    int ret= FALSE;
    if( data_num >0) {
        *data= osd_event[read];
        read++;
        if( read > bottom) {
            read = 0;
        }
        ret = TRUE;
        data_num--;
    }
    return ret;
}

#if 0
int main(void)
{
    int ret;
    OSD_EVENT data;
    initEvent();
    //data.keycode = 100;
    ret = readEvent( &data); printf("%d keycode=%d \n",ret ,data.keycode);
    ret = readEvent( &data); printf("%d keycode=%d \n",ret ,data.keycode);

    printf("-----\n");
    data.keycode = 0;
    for(int i=0; i<12 ; i++) {
        ret = writeEvent( data); printf("%d keycode=%d \n",ret ,data.keycode);
        data.keycode++;
    }
    printf("-----\n");
    for(int i=0; i<12 ; i++) {
        ret = readEvent( &data); printf("%d keycode=%d \n",ret ,data.keycode);
    }
}
#endif
