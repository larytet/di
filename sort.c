/*  
 *  sortc.c - a kernel module implementing a character device
 *  write() - write buffer 
*   read() - read back a sorted buffer 
 */
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>	
#include <linux/kernel.h>	
#include <linux/mutex.h>	
#include <linux/fs.h>
#include <linux/uaccess.h>

#define  DEVICE_NAME "sort"
#define  CLASS_NAME  "sort"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arkady");
MODULE_DESCRIPTION("A simple module");
MODULE_VERSION("0.1");
 
static int    majorNumber;

// Cutting corners: one thread at any time
DEFINE_MUTEX(sortMutex);
static char   message[256] = {0};
static short  size_of_message;

// Numebr of times the device is opened
static int    numberOpens = 0;              
static struct class*  sortClass  = NULL;
static struct device* sortDevice = NULL;
 
static int     dev_open(struct inode *, struct file *);
static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);


static struct file_operations fops =
{
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};

int init_module(void)
{
	printk(KERN_INFO "Sort is running\n");

    // Try to dynamically allocate a major number for the device
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0){
        printk(KERN_ALERT "Sort failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "Sort: registered correctly with major number %d\n", majorNumber);

    // Register the device class
    sortClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(sortClass)){          
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Sort: failed to register device class\n");
        return PTR_ERR(sortClass);      
    }
    printk(KERN_INFO "Sort: device class registered correctly\n");

    // Register the device driver
    sortDevice = device_create(sortClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(sortDevice)){              
        class_destroy(sortClass);       
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to create the device\n");
        return PTR_ERR(sortDevice);
    }
    mutex_init(&sortMutex);
    printk(KERN_INFO "Sort: device class created correctly\n"); // Made it! device was initialized
	return 0;
}

void cleanup_module(void)
{
	printk(KERN_INFO "Sort is going down\n");
}

static int dev_open(struct inode *inodep, struct file *filep){
   numberOpens++;
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset){
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   // Cuttting corners: ignore errors, len, offset, always copy the whole message
   if (len < size_of_message) {
       size_of_message = len;
   }
   copy_to_user(buffer, message, size_of_message);
   mutex_unlock(&sortMutex);
   return size_of_message;
}

static int compare_bytes(const void *lhs, const void *rhs) {
    u8 lhs_integer = *(u8 *)(lhs);
    u8 rhs_integer = *(u8 *)(rhs);

    if (lhs_integer < rhs_integer) return -1;
    if (lhs_integer > rhs_integer) return 1;
    return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset){
    mutex_lock(&sortMutex);
    size_of_message = len;
    // Cutting corners: I ignore offset
    if (size_of_message > sizeof(message)) {
        size_of_message = sizeof(message);
    }
    copy_from_user(message, buffer, size_of_message);
    sort(message, size_of_message, sizeof(u8), &compare_bytes, NULL);
    return size_of_message;
}

static int dev_release(struct inode *inodep, struct file *filep){
   numberOpens--;
   return 0;
}

static void __exit sort_exit(void){
   device_destroy(sortClass, MKDEV(majorNumber, 0));
   class_unregister(sortClass);
   class_destroy(sortClass);                     
   unregister_chrdev(majorNumber, DEVICE_NAME); 
   printk(KERN_INFO "Sort: getting down\n");
}

module_init(sort_init);
module_exit(sort_exit);