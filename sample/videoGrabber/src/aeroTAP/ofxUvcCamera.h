#ifndef _OFX_UVC_CAMERA_H_
#define _OFX_UVC_CAMERA_H_

enum io_method {
        IO_METHOD_READ,
        IO_METHOD_MMAP,
        IO_METHOD_USERPTR,
};

struct buffer {
        void   *start;
        long  length;
};

class ofxUvcCamera
{
public:
	ofxUvcCamera(const char* devName, int width, int height,  io_method method = IO_METHOD_MMAP);
	virtual ~ofxUvcCamera();

	int open_device(void);
	void close_device(void);
	void init_device(void);
	void uninit_device(void);
	void start_capturing(void);
	void stop_capturing(void);
	buffer* read_frame(void);
	int getWidth();
	int getHeight();

protected: 
	char dev_name[255];
	io_method io;
	int fd;
	buffer *buffers;
	unsigned int n_buffers;
	int force_format;
	int frame_count;
	int frame_width;
	int frame_height;
	int deviceOpened;
	int buffersCount;
	unsigned long currBufferId;

	int _read_frame(void);
	//void errno_exit(const char *s);
	int xioctl(int fh, int request, void *arg);
	//inline void process_image(const void *p, int size);
	//void mainloop(void);
	void init_read(unsigned int buffer_size);
	void init_mmap(void);
	void init_userp(unsigned int buffer_size);
};

#endif

