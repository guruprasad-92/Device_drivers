#include "device.h"
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/errno.h> 
#include <linux/module.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/kdev_t.h>

#define GSM_BANK      1
#define DEV_NAME    "gsm"
#define CLASS       "gsm-class"

struct class *gsm_class;
struct cdev gsm_dev[GSM_BANK];
dev_t major;
unsigned int first_minor = 0;

static struct file_operations gsm_dev_fops = 
{
    .owner = THIS_MODULE,
    .read = gsm_read,
};


int register_dev(void)
{
    printk(KERN_NOTICE "GSM: In register_dev().\n");
    /* Get the mazor and minor number */
    int res = 0;
    int rt_cdev_add = -1;
    dev_t cur_dev;

    int sts_mazor = -1;
    sts_mazor = alloc_chardev_region(&major,first_minor,GSM_BANK,DEV_NAME);
    if (sts_mazor != 0)
    {
        printk(KERN_ERR "GSM: Unable to get major number.\n");
    }
    else
    {
        printk(KERN_INFO "GSM: Major number = %d\n",MAJOR(major));

        gsm_class = class_create(THIS_MODULE, CLASS);

        for(int i=0; i<GSM_BANK; i++)
        {
            cdev_init(&gsm_dev[i], &gsm_dev_fops); // return void
            cur_dev = MKDEV( MAJOR(major), MINOR( major+1 ) );
            rt_cdev_add = cdev_add(&gsm_dev[i], cur_dev, 1);
            if(rt_cdev_add < 0)
            {
                printk(KERN_ERR "GSM: Unable to add the device.\n");
                return rt_cdev_add;
            }
            else
            {
                printk(KERN_INFO "GSM: The device has been added successfully.\n");
                device_create(gsm_class, NULL, cur_dev, NULL, DEV_NAME "%d",i);
            }
        }
    }
    return 0;
}

void unregister_dev(void)
{
    
}