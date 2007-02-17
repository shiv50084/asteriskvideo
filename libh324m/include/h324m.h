#ifndef _H324M_H_
#define _H324M_H_

#define MEDIA_AUDIO 	0
#define MEDIA_VIDEO 	1

#define CODEC_AMR  	0
#define CODEC_H263	1 


#ifdef __cplusplus
extern "C"
{
#endif

void*	H324MSessionCreate();
void	H324MSessionDestroy(void * id);

int	H324MSessionInit(void * id);
int	H324MSessionEnd(void * id);

int	H324MSessionRead(void * id,unsigned char *buffer,int len);
int	H324MSessionWrite(void * id,unsigned char *buffer,int len);

void*	H324MSessionGetFrame(void * id);
int	H324MSessionSendFrame(void * id,void *frame);

void* 	FrameCreate(int type,int codec, unsigned char * buffer, int len);
int 	FrameGetType(void* frame);
int 	FrameGetCodec(void* frame);
unsigned char * FrameGetData(void* frame);
int 	FrameGetLength(void *frame);
void 	FrameDestroy(void *frame);

#ifdef __cplusplus    
}
#endif

#endif
