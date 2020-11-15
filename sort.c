/*  
 *  sortc.c - a kernel module implementing a character device
 *  write() - write buffer 
*   read() - read back a sorted buffer 
 */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */

int init_module(void)
{
	printk(KERN_INFO "Sort is running\n");

	/* 
	 * A non 0 return means init_module failed; module can't be loaded. 
	 */
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Sort is going down\n");
}