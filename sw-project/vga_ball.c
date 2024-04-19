/* * Device driver for the VGA video generator
 *
 * A Platform device implemented using the misc subsystem
 *
 * Stephen A. Edwards
 * Columbia University
 *
 * References:
 * Linux source: Documentation/driver-model/platform.txt
 *               drivers/misc/arm-charlcd.c
 * http://www.linuxforu.com/tag/linux-device-drivers/
 * http://free-electrons.com/docs/
 *
 * "make" to build
 * insmod vga_ball.ko
 *
 * Check code style with
 * checkpatch.pl --file --no-tree vga_ball.c
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "vga_ball.h"

#define DRIVER_NAME "vga_ball"

/* Device registers */
// #define BG_RED(x) ((x)+32)
// #define BG_RED(x) ((x)+32)
// #define BG_GREEN(x) ((x)+32)
// #define BG_BLUE(x) ((x)+32)
//  #define Ball_RED(x) ((x)+32)    //new
//  #define Ball_GREEN(x) ((x)+32)  //new
//  #define Ball_BLUE(x) ((x)+32)   //new
#define Ball_X_1(x) ((x))   //new
#define Ball_X_2(x) ((x)+2)
#define Ball_X_3(x) ((x)+4)
#define Ball_X_4(x) ((x)+6)
#define Ball_X_5(x) ((x)+8)   //new
#define Ball_X_6(x) ((x)+10)
#define Ball_X_7(x) ((x)+12)
#define Ball_X_8(x) ((x)+14)
#define Ball_X_9(x) ((x)+16)   //new
#define Ball_X_10(x) ((x)+18)
#define Ball_X_11(x) ((x)+20)
#define Ball_X_12(x) ((x)+22)
#define Ball_X_13(x) ((x)+24)   //new
#define Ball_X_14(x) ((x)+26)
#define Ball_X_15(x) ((x)+28)
#define Ball_X_16(x) ((x)+30)
//#define Ball_Y(x) ((x)+6)   //new
#define status_track_1(x) ((x)+32)   //new
#define status_track_2(x) ((x)+34)
#define status_track_3(x) ((x)+36)
#define status_track_4(x) ((x)+38)
#define background_1(x) ((x)+40)
#define score_1(x) ((x)+42)
// #define Ball_RED(x) ((x)+10)    //new
// #define Ball_GREEN(x) ((x)+12)  //new
// #define Ball_BLUE(x) ((x)+14)   //new
// #define Ball_NUMBER(x) ((x)+16)
 
/*
 * Information about our device
 */
struct vga_ball_dev {
	struct resource res; /* Resource: our registers */
	void __iomem *virtbase; /* Where registers can be accessed in memory */
	note_position_t position; //-------------new----------
	node_status_t status;
	background_t background;
	total_score_t total_score;
} dev;

/*
 * Write segments of a single digit
 * Assumes digit is in range and the device information has been set up
 */
// static void write_background(vga_ball_color_t *background)
// {
// 	iowrite16(background->red, BG_RED(dev.virtbase) );
// 	iowrite16(background->green, BG_GREEN(dev.virtbase) );
// 	iowrite16(background->blue, BG_BLUE(dev.virtbase) );
// 	dev.background = *background;
// }

//  static void write_ball_color(vga_ball_color_t *ball)  //---------------new----------------------
//  {
//  	iowrite16(ball->red, Ball_RED(dev.virtbase) );
//  	iowrite16(ball->green, Ball_GREEN(dev.virtbase) );
//  	iowrite16(ball->blue, Ball_BLUE(dev.virtbase) );
//  	dev.ball = *ball;
//  }                                                    //------------------new---------------------

static void write_ball_position(note_position_t *position)  //---------------new----------------------
{
	iowrite16(position->X1_aix, Ball_X_1(dev.virtbase) );
	iowrite16(position->X2_aix, Ball_X_2(dev.virtbase) );
	iowrite16(position->X3_aix, Ball_X_3(dev.virtbase) );
	iowrite16(position->X4_aix, Ball_X_4(dev.virtbase) );
	iowrite16(position->X5_aix, Ball_X_5(dev.virtbase) );
	iowrite16(position->X6_aix, Ball_X_6(dev.virtbase) );
	iowrite16(position->X7_aix, Ball_X_7(dev.virtbase) );
	iowrite16(position->X8_aix, Ball_X_8(dev.virtbase) );
	iowrite16(position->X9_aix, Ball_X_9(dev.virtbase) );
	iowrite16(position->X10_aix, Ball_X_10(dev.virtbase) );
	iowrite16(position->X11_aix, Ball_X_11(dev.virtbase) );
	iowrite16(position->X12_aix, Ball_X_12(dev.virtbase) );
	iowrite16(position->X13_aix, Ball_X_13(dev.virtbase) );
	iowrite16(position->X14_aix, Ball_X_14(dev.virtbase) );
	iowrite16(position->X15_aix, Ball_X_15(dev.virtbase) );
	iowrite16(position->X16_aix, Ball_X_16(dev.virtbase) );
	//iowrite16(position->Y_aix, Ball_Y(dev.virtbase) );
	//iowrite8(position->blue, Ball_BLUE(dev.virtbase) );
	dev.position = *position;
}                                                    //------------------new---------------------   


static void write_ball_status(node_status_t *status)  //---------------new----------------------
{
	iowrite16(status->first_track, status_track_1(dev.virtbase) );
	iowrite16(status->second_track, status_track_2(dev.virtbase) );
	iowrite16(status->third_track, status_track_3(dev.virtbase) );
	iowrite16(status->fourth_track, status_track_4(dev.virtbase) );
	//iowrite16(position->Y_aix, Ball_Y(dev.virtbase) );
	//iowrite8(position->blue, Ball_BLUE(dev.virtbase) );
	dev.status = *status;
} 

static void write_background(background_t *background)  //---------------new----------------------
{
	iowrite16(background->back_status, background_1(dev.virtbase) );
	//iowrite16(position->Y_aix, Ball_Y(dev.virtbase) );
	//iowrite8(position->blue, Ball_BLUE(dev.virtbase) );
	dev.background = *background;
} 

static void write_ball_score(total_score_t *total_score)  //---------------new----------------------
{
	iowrite16(total_score->score, score_1(dev.virtbase) );
	//iowrite16(position->Y_aix, Ball_Y(dev.virtbase) );
	//iowrite8(position->blue, Ball_BLUE(dev.virtbase) );
	dev.total_score = *total_score;
} 

// static void write_ball_number(vga_ball_num_t *number)  
// {
// 	iowrite16(number->num, Ball_NUMBER(dev.virtbase) );
// 	//iowrite16(number->num2, Ball_NUMBER(dev.virtbase) );
// 	//iowrite16(number->num3, Ball_NUMBER(dev.virtbase) );
// 	dev.number = *number;
// }            

/*
 * Handle ioctl() calls from userspace:
 * Read or write the segments on single digits.
 * Note extensive error checking of arguments
 */
static long vga_ball_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
	// vga_ball_arg_t vla;
	// vga_ball_t bvla;  //new
	note_pos_t bpvla;  //new
	node_sta_t status_now;
	back_t background_now;
	total_sc_t score_now;
    //vga_ball_number_t bnumvla;

	switch (cmd) {
	// case VGA_BALL_WRITE_BACKGROUND:
	// 	if (copy_from_user(&vla, (vga_ball_arg_t *) arg,
	// 			   sizeof(vga_ball_arg_t)))
	// 		return -EACCES;
	// 	write_background(&vla.background);
	// 	break;

	// case VGA_BALL_READ_BACKGROUND:
	//   	vla.background = dev.background;
	// 	if (copy_to_user((vga_ball_arg_t *) arg, &vla,
	// 			 sizeof(vga_ball_arg_t)))
	// 		return -EACCES;
	// 	break;

	//  case VGA_BALL_WRITE_BALL:    //-----------------new-----------------------
	//    	//bvla.ball = dev.ball;
	//  	if (copy_from_user(&bvla, (vga_ball_t *) arg,
	//  			 sizeof(vga_ball_t)))
	//  		return -EACCES;
	//  	write_ball_color(&bvla.ball);
	//  	break;                   //----------------new------------------------

	case VGA_BALL_WRITE_POSITION:    //-----------------new-----------------------
	  	//bpvla.position = dev.position;
		if (copy_from_user(&bpvla, (note_pos_t *) arg,
				 sizeof(note_pos_t)))
			return -EACCES;
		write_ball_position(&bpvla.position);
		break;                   //----------------new--------------------
	case VGA_BALL_WRITE_STATUS:    //-----------------new-----------------------
	  	//bpvla.position = dev.position;
		if (copy_from_user(&status_now, (node_sta_t *) arg,
				 sizeof(node_sta_t)))
			return -EACCES;
		write_ball_status(&status_now.status);
		break; 
	case VGA_BALL_WRITE_BACKGROUND:    //-----------------new-----------------------
	  	//bpvla.position = dev.position;
		if (copy_from_user(&background_now, (back_t *) arg,
				 sizeof(back_t)))
			return -EACCES;
		write_background(&background_now.background);
		break; 
	case VGA_BALL_WRITE_SCORE:    //-----------------new-----------------------
	  	//bpvla.position = dev.position;
		if (copy_from_user(&score_now, (total_sc_t *) arg,
				 sizeof(total_sc_t)))
			return -EACCES;
		write_ball_score(&score_now.total_score);
		break; 
    // case VGA_BALL_NUMBER:    //-----------------new-----------------------
	//   	//bpvla.position = dev.position;
	// 	if (copy_from_user(&bnumvla, (vga_ball_num_t *) arg,
	// 			 sizeof(vga_ball_num_t)))
	// 		return -EACCES;
	// 	write_ball_number(&bnumvla.number);
	// 	break;   


	default:
		return -EINVAL;
	}

	return 0;
}

/* The operations our device knows how to do */
static const struct file_operations vga_ball_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl = vga_ball_ioctl,
};

/* Information about our device for the "misc" framework -- like a char dev */
static struct miscdevice vga_ball_misc_device = {
	.minor		= MISC_DYNAMIC_MINOR,
	.name		= DRIVER_NAME,
	.fops		= &vga_ball_fops,
};

/*
 * Initialization code: get resources (registers) and display
 * a welcome message
 */
static int __init vga_ball_probe(struct platform_device *pdev)
{
    //vga_ball_color_t beige = { 0xf9, 0xe4, 0xb7 };//0xf9, 0xe4, 0xb7
	int ret;

	/* Register ourselves as a misc device: creates /dev/vga_ball */
	ret = misc_register(&vga_ball_misc_device);

	/* Get the address of our registers from the device tree */
	ret = of_address_to_resource(pdev->dev.of_node, 0, &dev.res);
	if (ret) {
		ret = -ENOENT;
		goto out_deregister;
	}

	/* Make sure we can use these registers */
	if (request_mem_region(dev.res.start, resource_size(&dev.res),
			       DRIVER_NAME) == NULL) {
		ret = -EBUSY;
		goto out_deregister;
	}

	/* Arrange access to our registers */
	dev.virtbase = of_iomap(pdev->dev.of_node, 0);
	if (dev.virtbase == NULL) {
		ret = -ENOMEM;
		goto out_release_mem_region;
	}
        
	/* Set an initial color */
        //write_background(&beige);

	return 0;

out_release_mem_region:
	release_mem_region(dev.res.start, resource_size(&dev.res));
out_deregister:
	misc_deregister(&vga_ball_misc_device);
	return ret;
}

/* Clean-up code: release resources */
static int vga_ball_remove(struct platform_device *pdev)
{
	iounmap(dev.virtbase);
	release_mem_region(dev.res.start, resource_size(&dev.res));
	misc_deregister(&vga_ball_misc_device);
	return 0;
}

/* Which "compatible" string(s) to search for in the Device Tree */
#ifdef CONFIG_OF
static const struct of_device_id vga_ball_of_match[] = {
	{ .compatible = "csee4840,vga_ball-1.0" },
	{},
};
MODULE_DEVICE_TABLE(of, vga_ball_of_match);
#endif

/* Information for registering ourselves as a "platform" driver */
static struct platform_driver vga_ball_driver = {
	.driver	= {
		.name	= DRIVER_NAME,
		.owner	= THIS_MODULE,
		.of_match_table = of_match_ptr(vga_ball_of_match),
	},
	.remove	= __exit_p(vga_ball_remove),
};

/* Called when the module is loaded: set things up */
static int __init vga_ball_init(void)
{
	pr_info(DRIVER_NAME ": init\n");
	return platform_driver_probe(&vga_ball_driver, vga_ball_probe);
}

/* Calball when the module is unloaded: release resources */
static void __exit vga_ball_exit(void)
{
	platform_driver_unregister(&vga_ball_driver);
	pr_info(DRIVER_NAME ": exit\n");
}

module_init(vga_ball_init);
module_exit(vga_ball_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Stephen A. Edwards, Columbia University");
MODULE_DESCRIPTION("VGA ball driver");
