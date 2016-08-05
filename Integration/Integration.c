#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>

#include "uart_api.h"
#include "robot_protocol.h"
#include "amazon2_sdk.h"
#include "graphic_api.h"


int start(void)
{
	// var declaration
	U16* raw_videodata = (U16*)malloc(180*120*2);
	U16* grayed_data;
	YUV* yuv_PixelData = (YUV*)malloc(12);
	uvset* decision_queue = (uvset*)malloc(8*5);
	uvset* spec = (uvset*)malloc(8);
	S16* p_radius = malloc(4*sizeof(S16));
	U16* p_theta = malloc(4*sizeof(U16));
	int i = 1000;

	// Sobel / Gaussian mask
	S16 maskX[9] = {1, 2, 1, 0, 0, 0, -1, -2, -1};
	S16 maskY[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1};
	S32 gaussian_mask[9] = {113, 838, 113, 838, 6196, 838, 113, 838, 113};

	memset(p_radius, 0, 30*sizeof(S16));
	memset(p_theta, 0, 30*sizeof(U16));
	memset(decision_queue, 0, 10*sizeof(float));
	memset(spec, 0, 2*sizeof(float));
	printf("Start.\n");

	// On each frame
	while(i--) {
		clear_screen();
		printf("on frame %d,\n", i);
		// Raw fpga video data
		read_fpga_video_data(raw_videodata);
		printf("1");
		// 1. Gray scaled data
		grayed_data = gray_scale(raw_videodata);
		printf("2");
		// 2. gaussian masking from 1.
		/*mask_filtering(grayed_data, gaussian_mask);
		draw_fpga_video_data(grayed_data, 10, 10);
		printf("3");
		// 3. sobel masking from 2.
		sobel_mask_filtering(grayed_data, maskX, maskY, 3);
		draw_fpga_video_data(grayed_data, 10, 290);
		printf("4");
		// 4. hough transform from 3.
		hough_lines(grayed_data, 13, 5.0, p_radius, p_theta);
		draw_fpga_video_data(grayed_data, 10, 150);
		printf("5");
		flip();*/
		if(i == 900) break;
	}
	free(raw_videodata);
	free(yuv_PixelData);
	free(decision_queue);
	free(spec);
	free(p_radius);
	free(p_theta);
	printf("end.");

	return 0;
}

#include <termios.h>
static struct termios inittio, newtio;

void init_console(void)
{
    tcgetattr(0, &inittio);
    newtio = inittio;
    newtio.c_lflag &= ~ICANON;
    newtio.c_lflag &= ~ECHO;
    newtio.c_lflag &= ~ISIG;
    newtio.c_cc[VMIN] = 1;
    newtio.c_cc[VTIME] = 0;

    cfsetispeed(&newtio, B115200);

    tcsetattr(0, TCSANOW, &newtio);
}

int main(void)
{
	init_console();
	if ((uart_open() < 0) || (open_graphic() < 0)) return EXIT_FAILURE;
	direct_camera_display_off();
	uart_config(UART1, 9600, 8, UART_PARNONE, 1);
	start();
	uart_close();
	close_graphic();
	return 0;
}
