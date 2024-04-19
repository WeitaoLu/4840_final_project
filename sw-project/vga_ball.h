#ifndef _VGA_BALL_H
#define _VGA_BALL_H

#include <linux/ioctl.h>

// typedef struct {
// 	unsigned char red, green, blue;
// } vga_ball_color_t;
  

// typedef struct {
//   vga_ball_color_t background;
// } vga_ball_arg_t;

//  typedef struct {
//    vga_ball_color_t ball;
//  } vga_ball_t;  //new

typedef struct {    //new
	short  X1_aix, X2_aix, X3_aix, X4_aix, X5_aix, X6_aix, X7_aix, X8_aix
      ,X9_aix, X10_aix, X11_aix, X12_aix, X13_aix, X14_aix, X15_aix, X16_aix;
} note_position_t; //new


typedef struct {
    short fourth_track, third_track, second_track, first_track;
} node_status_t;

typedef struct {
    node_status_t status;
} node_sta_t;

typedef struct {
  note_position_t position;
} note_pos_t;  //new


typedef struct {
    short back_status;
}background_t;

typedef struct {
    background_t background;
} back_t;

typedef struct {
    short score;
} total_score_t;

typedef struct {
    total_score_t total_score;
} total_sc_t;


//unsigned char num1, num2, num3;
// } vga_ball_num_t; //new

// typedef struct {    //new
// 	vga_ball_num_t number;
// } vga_ball_number_t; //new

#define VGA_BALL_MAGIC 'q'

/* ioctls and their arguments */
// #define VGA_BALL_WRITE_BACKGROUND _IOW(VGA_BALL_MAGIC, 1, vga_ball_arg_t *)
// #define VGA_BALL_READ_BACKGROUND  _IOR(VGA_BALL_MAGIC, 2, vga_ball_arg_t *)
// #define VGA_BALL_WRITE_BALL       _IOW(VGA_BALL_MAGIC, 4, vga_ball_t *)//new
#define VGA_BALL_WRITE_POSITION       _IOW(VGA_BALL_MAGIC, 3, note_pos_t *)//new
#define VGA_BALL_WRITE_STATUS       _IOW(VGA_BALL_MAGIC, 4, node_sta_t *)//new
#define VGA_BALL_WRITE_BACKGROUND       _IOW(VGA_BALL_MAGIC, 5, back_t *)//new
#define VGA_BALL_WRITE_SCORE     _IOW(VGA_BALL_MAGIC, 6, total_score_t *)//new

//#define VGA_BALL_NUMBER       _IOW(VGA_BALL_MAGIC, 5, vga_ball_number_t *)

#endif
