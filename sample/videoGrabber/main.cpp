/*
 *  V4L2 video capture example
 *
 *  This program can be used and distributed without restrictions.
 *
 *      This program is provided with the V4L2 API
 * see https://linuxtv.org/docs.php for more information
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include <errno.h>
#include <fcntl.h> /* low-level i/o */
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <linux/videodev2.h>

using namespace std;

#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer {
	void *start;
	size_t length;
};

static char *dev_name[2];
static int fd[2] = {-1,-1};
struct buffer *buffers[2];
static unsigned int n_buffers[2];
static int out_buf[2];
static int force_format = 1;

static void errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

static int xioctl(int fh, int request, void *arg)
{
	int r;

	do {
		r = ioctl(fh, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}

static void getProperty(int fd, int property)
{
   v4l2_queryctrl queryControl;
   queryControl.id = property;
   if ( xioctl( fd, VIDIOC_QUERYCTRL, &queryControl ) < 0 )
   {
	   if ( errno != EINVAL ) {
		printf("ioctl VIDOC_QUERYCTRL error !  fd %d\n",fd);
	   }
	   else {
		printf("getProperty error !  fd %d\n",fd);
		}
   }
   else
   {
        if ( ( queryControl.type & ( V4L2_CTRL_TYPE_BOOLEAN | V4L2_CTRL_TYPE_INTEGER ) ) != 0 )
        {
            printf( "property: %d, min: %d, max: %d, step: %d, def: %d, type: %s \n ", property,
                    queryControl.minimum, queryControl.maximum, queryControl.step, queryControl.default_value,
                    ( queryControl.type & V4L2_CTRL_TYPE_BOOLEAN ) ? "bool" : "int" );

        }
	   
   }

}


static void init_mmap(int cam)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd[cam], VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
			                "memory mapping\n",
			        dev_name[cam]);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n",
		        dev_name[cam]);
		exit(EXIT_FAILURE);
	}

	buffers[cam] = (struct buffer *)calloc(req.count, sizeof(*buffers[cam]));

	if (!buffers[cam]) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers[cam] = 0; n_buffers[cam] < req.count; ++n_buffers[cam]) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers[cam];

		if (-1 == xioctl(fd[cam], VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		buffers[cam][n_buffers[cam]].length = buf.length;
		buffers[cam][n_buffers[cam]].start =
		    mmap(NULL /* start anywhere */,
		         buf.length,
		         PROT_READ | PROT_WRITE /* required */,
		         MAP_SHARED /* recommended */,
		         fd[cam], buf.m.offset);

		if (MAP_FAILED == buffers[cam][n_buffers[cam]].start)
			errno_exit("mmap");
	}
}

static void open_device(int cam)
{
	struct stat st;

	if (-1 == stat(dev_name[cam], &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n",
		        dev_name[cam], errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name[cam]);
		exit(EXIT_FAILURE);
	}

	fd[cam] = open(dev_name[cam], O_RDWR /* required */ | O_NONBLOCK, 0);

	if (-1 == fd[cam]) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n",
		        dev_name[cam], errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("open device %s fd:%d\n",dev_name[cam],fd[cam]);
}

static void init_device(int cam)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;

	if (-1 == xioctl(fd[cam], VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n",
			        dev_name[cam]);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n",
		        dev_name[cam]);
		exit(EXIT_FAILURE);
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		fprintf(stderr, "%s does not support streaming i/o\n",
		        dev_name[cam]);
		exit(EXIT_FAILURE);
	}

	/* Select video input, video standard and tune here. */

	CLEAR(cropcap);

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (0 == xioctl(fd[cam], VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect; /* reset to default */

		if (-1 == xioctl(fd[cam], VIDIOC_S_CROP, &crop)) {
			switch (errno) {
			case EINVAL:
				/* Cropping not supported. */
				break;
			default:
				/* Errors ignored. */
				break;
			}
		}
	} else {
		/* Errors ignored. */
	}

	CLEAR(fmt);

	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (force_format) {
		fmt.fmt.pix.width = 640;
		fmt.fmt.pix.height = 480;
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
		fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

		//		fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

		if (-1 == xioctl(fd[cam], VIDIOC_S_FMT, &fmt))
			errno_exit("VIDIOC_S_FMT");

		/* Note VIDIOC_S_FMT may change width and height. */
	} else {
		/* Preserve original settings as set by v4l2-ctl for example */
		if (-1 == xioctl(fd[cam], VIDIOC_G_FMT, &fmt))
			errno_exit("VIDIOC_G_FMT");
	}

	/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	init_mmap(cam);
}

static void start_capturing(int cam)
{
	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < n_buffers[cam]; ++i) {
		struct v4l2_buffer buf;

		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == xioctl(fd[cam], VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd[cam], VIDIOC_STREAMON, &type))
		errno_exit("VIDIOC_STREAMON");
}

void process_image(const void *p, int size)
{
    printf("Buffer size: %d\n",size);
#if 0
	fwrite(p, size, 1, stdout);

	fflush(stderr);
	fprintf(stderr, ".");
	fflush(stdout);
#endif
}
static int read_frame(int cam)
{
	struct v4l2_buffer buf;

	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 == xioctl(fd[cam], VIDIOC_DQBUF, &buf)) {
		switch (errno) {
		case EAGAIN:
			return 0;

		case EIO:
		/* Could ignore EIO, see spec. */

		/* fall through */

		default:
			errno_exit("VIDIOC_DQBUF");
		}
	}

	assert(buf.index < n_buffers[cam]);
#if 0
	struct parrot_image_meta meta = {
	    .width = 640,
	    .height = 480,
	    .image_format = YUYV, /* YUYV 4.2.2 */
	    .buffer_size = buf.bytesused,
	};
#endif

	//	write(1, buffers[buf.index].start, buf.bytesused);
    process_image(buffers[cam][buf.index].start, buf.bytesused);

	if (-1 == xioctl(fd[cam], VIDIOC_QBUF, &buf))
		errno_exit("VIDIOC_QBUF");

	return 1;
}


static void mainloop(void)
{
	unsigned int count = 0;

    int cam=0;
	for (;;) {
		fd_set fds;
		struct timeval tv;
		int r;

		FD_ZERO(&fds);
		FD_SET(fd[cam], &fds);

		/* Timeout. */
		tv.tv_sec = 2;
		tv.tv_usec = 0;

		r = select(fd[cam] + 1, &fds, NULL, NULL, &tv);

		if (-1 == r) {
			if (EINTR == errno)
				continue;
			errno_exit("select");
		}

		if (0 == r) {
			fprintf(stderr, "select timeout\n");
			exit(EXIT_FAILURE);
		}

		read_frame(cam);
		//			break;
		/* EAGAIN - continue select loop. */
	}
}

static void stop_capturing(void)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(fd[0], VIDIOC_STREAMOFF, &type))
		errno_exit("VIDIOC_STREAMOFF");
	if (-1 == xioctl(fd[1], VIDIOC_STREAMOFF, &type))
		errno_exit("VIDIOC_STREAMOFF");
}

static void uninit_device(void)
{
	unsigned int i;

	for (i = 0; i < n_buffers[0]; ++i)
		if (-1 == munmap(buffers[0][i].start, buffers[0][i].length))
			errno_exit("munmap");
	free(buffers[0]);

	for (i = 0; i < n_buffers[1]; ++i)
		if (-1 == munmap(buffers[1][i].start, buffers[1][i].length))
			errno_exit("munmap");

	free(buffers[1]);
}

static void close_device(void)
{
	if ( fd[0]!=-1 && -1 == close(fd[0]))
		errno_exit("close device0");

	fd[0] = -1;
	if ( fd[1]!=01 && -1 == close(fd[1]))
		errno_exit("close device1");

	fd[1] = -1;
}

int main(int argc, char **argv)
{
    	dev_name[0] = (char*)"/dev/video0";
    	dev_name[1] = (char*)"/dev/video1";

    printf("videoGrabber [Color Device] [Depth Device]\n\n");
    if ( argc>2 )
    {
        dev_name[0] = argv[1];
//		printf("argv1 %s\n",argv[1]);
	}
    if ( argc>3 )
        dev_name[1] = argv[2];
    

	open_device(0);
//	printf("open device %s done\n",dev_name[0]);
	init_device(0);
	printf("init device done\n");

	getProperty(fd[0], V4L2_CID_BRIGHTNESS);

	start_capturing(0);
	printf("start done\n");

	printf("main loop\n");
	mainloop();

	stop_capturing();
	uninit_device();
	close_device();

	return 0;
}
