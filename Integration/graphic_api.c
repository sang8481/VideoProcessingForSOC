#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>
#include "graphic_api.h"

#define PRINTLINE	printf("%s , %d \r\n",__FILE__,__LINE__)
#define PRINTVAR(A)	do{int ch;printf("%s=0x%x(%d)\r\n",#A,A,A);ch = getchar();} while (0);

#define AMAZON2_GRAPHIC_NAME  "/dev/amazon2_graphic"

#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		480

static int graphic_handle = -1;

static float __sintab[91] =
{
	0.0000000000000000f,	0.0174524064372835f,	0.0348994967025010f,	0.0523359562429438f,
	0.0697564737441253f,	0.0871557427476582f,	0.1045284632676535f,	0.1218693434051475f,
	0.1391731009600654f,	0.1564344650402309f,	0.1736481776669303f,	0.1908089953765448f,
	0.2079116908177593f,	0.2249510543438650f,	0.2419218955996677f,	0.2588190451025208f,
	0.2756373558169992f,	0.2923717047227367f,	0.3090169943749474f,	0.3255681544571567f,
	0.3420201433256687f,	0.3583679495453003f,	0.3746065934159120f,	0.3907311284892738f,
	0.4067366430758002f,	0.4226182617406994f,	0.4383711467890774f,	0.4539904997395468f,
	0.4694715627858908f,	0.4848096202463370f,	0.5000000000000000f,	0.5150380749100542f,
	0.5299192642332050f,	0.5446390350150271f,	0.5591929034707468f,	0.5735764363510461f,
	0.5877852522924731f,	0.6018150231520483f,	0.6156614753256583f,	0.6293203910498375f,
	0.6427876096865393f,	0.6560590289905073f,	0.6691306063588582f,	0.6819983600624985f,
	0.6946583704589973f,	0.7071067811865475f,	0.7193398003386511f,	0.7313537016191705f,
	0.7431448254773942f,	0.7547095802227720f,	0.7660444431189780f,	0.7771459614569709f,
	0.7880107536067220f,	0.7986355100472928f,	0.8090169943749474f,	0.8191520442889918f,
	0.8290375725550417f,	0.8386705679454240f,	0.8480480961564260f,	0.8571673007021123f,
	0.8660254037844386f,	0.8746197071393958f,	0.8829475928589269f,	0.8910065241883679f,
	0.8987940462991670f,	0.9063077870366500f,	0.9135454576426009f,	0.9205048534524403f,
	0.9271838545667874f,	0.9335804264972017f,	0.9396926207859084f,	0.9455185755993168f,
	0.9510565162951536f,	0.9563047559630355f,	0.9612616959383189f,	0.9659258262890683f,
	0.9702957262759965f,	0.9743700647852352f,	0.9781476007338056f,	0.9816271834476640f,
	0.9848077530122081f,	0.9876883405951377f,	0.9902680687415703f,	0.9925461516413220f,
	0.9945218953682733f,	0.9961946980917455f,	0.9975640502598242f,	0.9986295347545739f,
	0.9993908270190957f,	0.9998476951563912f,	1.0000000000000000f,
};

void draw_rectfill(int x, int y, int w, int h, EGL_COLOR clr)
{
	if (graphic_handle < 0)
		return;
	DrawRectFillArg arg;
	arg.rect.x = x;
	arg.rect.y = y;
	arg.rect.w = w;
	arg.rect.h = h;
	arg.clr = clr;
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_RECT_FILL, &arg);
}

void clear_screen(void)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_CLEAR_SCREEN, 0);
}

void flip(void)
{
	if (graphic_handle < 0)
		return;
	ioctl(graphic_handle, AMAZON2_IOCTL_FLIP, 0);
}
void flipwait(void)
{
	if (graphic_handle < 0)
		return;
	ioctl(graphic_handle, AMAZON2_IOCTL_FLIPWAIT, 0);
}

SURFACE* create_surface(int w, int h, int bpp)
{
	SURFACE* surf;
	CreateSurfaceArg arg;
	arg.w = w;
	arg.h = h;
	arg.bpp = bpp;
	surf = (SURFACE*)ioctl(graphic_handle, AMAZON2_IOCTL_CREATE_SURFACE, &arg);
	return surf;
}

void release_surface(SURFACE* surf)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_RELEASE_SURFACE, surf);
}
void draw_surface(SURFACE* src_surf, int dx, int dy)
{
	DrawSurfaceArg ar;
	ar.surf = src_surf;
	ar.dx = dx;
	ar.dy = dy;
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_SURFACE, &ar);
}
void draw_surfacerect(SURFACE* src_surf, int dx, int dy, EGL_RECT* pRect)
{
	DrawSurfaceRectArg ar;
	ar.surf = src_surf;
	ar.dx = dx;
	ar.dy = dy;
	memcpy(&ar.rect, pRect, sizeof(EGL_RECT));
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_SURFACE_RECT, &ar);
}
void draw_surface_scalerectrect(SURFACE* src_surf, EGL_RECT* pDestRect, EGL_RECT* pSrcRect)
{
	DrawSurfaceScaleRectArg ar;
	ar.surf = src_surf;
	memcpy(&ar.dest_rect, pDestRect, sizeof(EGL_RECT));
	memcpy(&ar.surf_rect, pSrcRect, sizeof(EGL_RECT));
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_SURFACE_RECT, &ar);
}

static float mysin(U32 angle)
{
	angle %= 360;

	if(angle <= 90)
		return __sintab[angle];
	else if(angle <= 180)
		return __sintab[180 - angle];
	else if(angle <= 270)
		return -__sintab[angle - 180];
	else
		return -__sintab[360 - angle];
}

static float mycos(U32 angle)
{
	return mysin(angle + 90);
}

int draw_rotate_value(int cdx, int cdy, int ctx, int cty, float zoom, unsigned int angle, DrawRaw_value* draw_value)
{
	float sinval, cosval;

	int InitDX, InitDY;
	int EndX, EndY;
	int InitSX, InitSY;
	int dxSx, dxSy;
	int dySx, dySy;
	int dx,dy;
	int x, y;
	int cosa;
	int sina;
	int rhzoom;
	long tx,ty;

	x = y = 0;
	dx = SCREEN_WIDTH;	// screen width (320)
	dy = SCREEN_HEIGHT;	// screen height (480)

	sinval = mysin(angle);
	cosval = mycos(angle);

	tx = (-cdx/zoom) * cosval + (-cdy/zoom) * sinval;
	ty = (cdx/zoom) * sinval +  (-cdy/zoom) * cosval;

	if( zoom<=0 )   rhzoom = 0;
	else            rhzoom = (int)((float)(1<<9)/zoom);
	cosa = (S32)(rhzoom * cosval);
	sina = (S32)(rhzoom * sinval);

	if( dx <= 0 || dy <= 0 )
		return -1;

	InitDX 	= x;
	InitDY 	= y;
	EndX	= x+dx-1;
	EndY	= y+dy-1;

	InitSX  = (x+tx+ctx)*512;
	dxSx    = cosa;
	dxSy    = -sina;

	InitSY  = (y+ty+cty)*512;
	dySx    = sina;
	dySy    = cosa;

	draw_value->InitDX=InitDX;
	draw_value->InitDY=InitDY;
	draw_value->EndX=EndX;
	draw_value->EndY=EndY;
	draw_value->InitSX=InitSX;
	draw_value->InitSY=InitSY;
	draw_value->dxSx=dxSx;
	draw_value->dxSy=dxSy;
	draw_value->dySx=dySx;
	draw_value->dySy=dySy;

	return 0;
}

void read_fpga_video_data(U16* buf)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_READ_FPGA_VIDEO_DATA, buf);
}

void draw_fpga_video_data(U16* buf, int dx, int dy)
{
	DrawFPGADataArg ar;
	ar.buf = buf;
	ar.dx = dx;
	ar.dy = dy;
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_FPGA_VIDEO_DATA, &ar);
}

void draw_fpga_video_data_full(U16* buf)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_FPGA_VIDEO_DATA_FULL, buf);
}

void draw_img_from_buffer(U16* buf, int cdx, int cdy, int ctx, int cty, float zoom, int angle)
{
	DrawRaw_value draw_value;
	draw_value.imgbuf = buf;
	draw_rotate_value(cdx, cdy, ctx, cty, zoom, angle, &draw_value);

	ioctl(graphic_handle, AMAZON2_IOCTL_DRAW_IMG_FROM_BUFFER, &draw_value);
}

void direct_camera_display_on(void)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_CAM_DISP_ON, 0);
}

void direct_camera_display_off(void)
{
	ioctl(graphic_handle, AMAZON2_IOCTL_CAM_DISP_OFF, 0);
	clear_screen();
}

int direct_camera_display_stat(void)
{
	return ioctl(graphic_handle, AMAZON2_IOCTL_CAM_DISP_STAT, 0);
}

int open_graphic(void)
{
	if ((graphic_handle = open(AMAZON2_GRAPHIC_NAME, O_RDWR)) < 0) {
		printf("Open Error %s\n", AMAZON2_GRAPHIC_NAME);
		return -1;
	}
	return 0;
}

void close_graphic(void)
{
	if (graphic_handle != -1)
		close(graphic_handle);
}

/////////////////////////////
// Assume 180x120x16bit pixel
void color_ref(U16* buf, RGB565* pixel, int x, int y){
	// Clip
	if(x < 0 || x >= 180){
		printf("pixel index out of range - X:%d\n", x);
		return;
	}else if (y < 0 || y >= 120){
		printf("pixel index out of range - Y:%d\n", y);
		return;
	}else{
        printf("pixel value : 0x%X\n", buf[(119 - y)*180 + x]);
		pixel->b = BLUE_VALUE_IN565(buf[(119 - y)*180 + x]);
        pixel->g = GREEN_VALUE_IN565(buf[(119 - y)*180 + x]);
		pixel->r = RED_VALUE_IN565(buf[(119 - y)*180 + x]);
		return;
	}
}

// Assume 180x120x16bit pixel
void avr_rbg(U16* buf, RGB565* pixel){
	int x, y;
	U32 rsum = 0, bsum = 0, gsum = 0;
	for(y = 120-1; y >= 0; y--){
		for(x = 0; x < 180; x++){
			bsum += BLUE_VALUE_IN565(buf[y*180 + x]);
			gsum += GREEN_VALUE_IN565(buf[y*180 + x]);
			rsum += RED_VALUE_IN565(buf[y*180 + x]);
		}
	}
	pixel->b = bsum/(180*120);
	pixel->g = gsum/(180*120*2);
	pixel->r = rsum/(180*120);
	return;
}
void rgb2yuv(U16* buf, YUV* yuv_pixel){
    int width = 180;
    int height = 120;
    int r,c;
    float tmp,Y,U,V;
    U16 red = 0, green = 0, blue = 0;
    float Ysum = 0, Usum = 0, Vsum = 0;
    float MYsum = 0, MUsum = 0, MVsum = 0;

    for(r = 0; r < height; r++){
	for(c = 0; c < width; c++){
	 	red = RED_VALUE_IN565(buf[180*r + c]);
	 	blue = BLUE_VALUE_IN565(buf[180*r + c]);
	 	green = GREEN_VALUE_IN565(buf[180*r + c]);

	     Y = 0.299*red + 0.293*green + 0.114*blue;
	     U = 0.492*(0.886*blue - 0.299*red - 0.293*green);
	     V = 0.877*(0.701*red - 0.293*green - 0.114*blue);

	     Ysum += Y;
	     Usum += U;
	     Vsum += V;

		// (30<=r<60) && (45<=c<135) : total 270 pixels
		if( (c >= (width/4)) && (c < (3*width/4)) && (r >= height/4) && (r < height/2)){
			MYsum +=Y;
			MUsum +=U;
			MVsum +=V;
		}
	}
	}
	tmp = MYsum / (180 * 120);
	yuv_pixel->Y = tmp;
	tmp = MUsum / (180 * 120);
	yuv_pixel->U = tmp;
	tmp = MVsum / (180 * 120);
	yuv_pixel->V = tmp;
}

void decision_queue_push(uvset* decision_queue, int size, float u, float v){
	int i;
	// if queue not full
	if(decision_queue[size-1].u == 0){
		for(i = 0; i < size; i++){
			if(decision_queue[i].u == 0){
				decision_queue[i].u = u;
				decision_queue[i].v = v;
				break;
			}
		}
	}
	else{
		for(i = 0; i < size-1; i++){
			decision_queue[i] = decision_queue[i+1];
		}
		decision_queue[size-1].u = u;
		decision_queue[size-1].v = v;
	}
}

void decision_queue_avg(uvset* decision_queue, int size, uvset* ret){
	float usum = 0, vsum = 0;
	int i, item_count = 0;
	for(i = 0; i < size; i++){
		if(decision_queue[i].u != 0){
			usum += decision_queue[i].u;
			vsum += decision_queue[i].v;
			item_count++;
		}
	}
	if(item_count == 0) return;

	ret->u = usum/item_count;
	ret->v = vsum/item_count;
}

// return allocated new image
U16* gray_scale(U16* buf){
	U16* grayed_image = (U16*)malloc(sizeof(U16)*180*120);
	int width = 180;
	int height = 120;
	double y_scale = 0; // Use 5-bit gray scale
	int r, c;
	for(r = 0; r < height; r++){
	for(c = 0; c < width; c++){
		y_scale =
		 (0.299*RED_VALUE_IN565(buf[180*r + c])
		+ 0.2935*GREEN_VALUE_IN565(buf[180*r + c])
		+ 0.114*BLUE_VALUE_IN565(buf[180*r + c]));
		grayed_image[180*r + c] = (U16)(((U16)y_scale<<11) | ((U16)y_scale<<6) | ((U16)y_scale));
	}
	}
	return grayed_image;
}

// Usage : mask_filtering(buf, mask_type);
// Assume gray-scaled image, 3x3 mask as input
// Dividing by summation of all array elements is required.
// Return filter-applied image pointer
void mask_filtering(U16* buf, S32* mask){
	int width = 180;
	int height = 120;
	U16 new_image[180*120];
	int half_masksize = 3 / 2;

	int r, c;
	for(r = 0; r < height; r++){
	for(c = 0; c < width; c++){
		U16 sum_of_pixel_count = 0;
		S32 sum_of_pixel_value = 0;

		int x, y;
		for(y = -half_masksize; y <= half_masksize; y++){
		for(x = -half_masksize; x <= half_masksize; x++){

			int px = c + x;
			int py = r + y;
			// If (r+dx, c+dy) pixel is rocated in valid range
			if((px >= 0) && (px < width) && (py >= 0) && (py < height)){
				sum_of_pixel_value += (BLUE_VALUE_IN565(buf[py*180 + px])*mask[3*(y+1) + (x+1)]);
				sum_of_pixel_count++;
				//if(r == 60 && c == 100) printf("px : %d, py : %d, pixelvalue : %d\n", px, py, sum_of_pixel_value);
			}
		}
		}
		sum_of_pixel_value = (U16)(sum_of_pixel_value / (10000.0));
		new_image[r*180 + c] = ((sum_of_pixel_value) | (sum_of_pixel_value<<6) | (sum_of_pixel_value<<11));
		/*if(r == 60 && c == 100) {
			printf("pcnt : %d\n", sum_of_pixel_count);
			printf("sum_of_pixel_value:%d\n", BLUE_VALUE_IN565(new_image[r*180 + c]));
		}*/
	}
	}
	memcpy(buf, new_image, sizeof(U16)*180*120);
}

// Usage : sobel_mask_filtering(buf, maskX, maskY, masksize, divisor);
// Assume gaussian_masked from gray-scaled image, two 3x3 mask as input
// Return filter-applied image pointer
void sobel_mask_filtering(U16* buf, S16* maskX, S16* maskY, int masksize){
	int width = 180;
	int height = 120;
	int half_masksize = masksize / 2;
	U16 new_image[180*120];
	int r, c;

	// Sobel mask : except edges of video
	for(r = 1; r < height - 1; r++){
	for(c = 1; c < width - 1; c++){
		S16 sum_of_pixel_valueX = 0;
		S16 sum_of_pixel_valueY = 0;
		U16 absXY = 0;

		int x, y;
		for(y = -half_masksize; y <= half_masksize; y++){
		for(x = -half_masksize; x <= half_masksize; x++){
			int px = c + x;
			int py = r + y;

			// If (r+dx, c+dy) pixel is rocated in valid range
			if((px >= 0) && (px < width) && (py >= 0) && (py < height)){
				sum_of_pixel_valueX += BLUE_VALUE_IN565(buf[py*180 + px])*maskX[(y + 1)*3 + (x + 1)];
				sum_of_pixel_valueY += BLUE_VALUE_IN565(buf[py*180 + px])*maskY[(y + 1)*3 + (x + 1)];
			}
		}
		}
		absXY = (U16)sqrt(((double)sum_of_pixel_valueX*sum_of_pixel_valueX + sum_of_pixel_valueY*sum_of_pixel_valueY)/10);
		new_image[r*180 + c] = ((absXY) | (absXY<<6) | (absXY<<11));

	}// for 'c'
	}// for 'r'
	memcpy(buf, new_image, sizeof(U16)*180*120);
}

void hough_lines(U16* buf, U16 threshold_value,
                double resolution, S16* p_radius, U16* p_theta){
	int width = 180, height = 120, r, c, i, j, k;
	U16 diagH = (U16)(sqrt((double)(180*180 + 120*120)));
	U16 diag = diagH*2;
	U16 res_step = (U16)(180/resolution); // In resolution 1, each step has 1 degree.
	U16 num_trans = diag*res_step;
	U16 hough_space[num_trans];
	U16 theta;
	memset(hough_space, 0, num_trans*sizeof(U16));

	for(r = 5; r < height - 5; r++){
	for(c = 5; c < width - 5; c++){

		int distance;
		// At each edge pixels
		if(BLUE_VALUE_IN565(buf[180*r + c]) > threshold_value){
			for(theta = 0; theta < 180; theta += (U16)resolution){
				distance = (int)(c*mysin(theta) + r*mycos(theta) + diagH + 0.5);
				hough_space[distance*res_step + (U16)(theta/resolution)]++;
			}
		}

	}
	}

	int highest_voted_pixel[6] = {0, 0, 0, 0, 0, 0};
	int highest_voted_pixel_index[6] = {0, 0, 0, 0, 0, 0};

	for(i = 0; i < num_trans; i++){
		if(hough_space[i] >= highest_voted_pixel[0]){
			// push highest_voted_pixel, index array
			for(j = 5; (j >= 0) && (hough_space[i] >= highest_voted_pixel[j]); j--){
				for(k = 1; k < j + 1; k++){
					highest_voted_pixel[k-1] = highest_voted_pixel[k];
					highest_voted_pixel_index[k-1] = highest_voted_pixel_index[k];
				}
				highest_voted_pixel[j] = hough_space[i];
				highest_voted_pixel_index[j] = i;
				break;
			}
		}
	}

	for(i = 0; i < 6; i++){
		p_radius[i] = (S16)(highest_voted_pixel_index[i]/res_step);
		p_theta[i] = (highest_voted_pixel_index[i] - p_radius[i]*res_step)*resolution;
		p_radius[i] -= diagH;
	}

	for(i = 0; i < 6; i++){
		draw_line(buf, p_radius[i], p_theta[i]);
	}
	/*for(line_count = 0; line_count < num_line; line_count++){
		printf("line no. %d  p_r : %d / p_t : %d\n", line_count, p_radius[line_count], p_theta[line_count]);
	}*/



}
// a = sin(theta)/cos(theta)
void draw_line(U16* buf, S16 r, U16 theta){
	S16 x, y, i;
	U16 blue = 0x1f;
	if(theta == 90){
		return;
	}
	for(x = 0; x < 180; x++){
		y = (S16)(-(mysin(theta)/mycos(theta))*x + r/mycos(theta));
		if(y >= 0 && y < 120) {
			for(i = -1; i < 2; i++){
				buf[180*(y + i) + x] = blue;
			}
		}
	}
}
