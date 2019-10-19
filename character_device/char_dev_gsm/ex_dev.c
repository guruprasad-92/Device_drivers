#define EEP_NBANK 8
#define EEP_DEVICE_NAME "eep-mem"
#define EEP_CLASS "eep-class"

struct class *eep_class;
struct cdev eep_cdev[EEP_NBANK];
dev_t dev_num;

static int __init my_init(void)
{
    int i;
    dev_t curr_dev;
    
    /* Request the kernel for EEP_NBANK devices */
    alloc_chrdev_region(&dev_num, 0, EEP_NBANK, EEP_DEVICE_NAME);
    
    /* Let's create our device's class, visible in /sys/class */    
    eep_class = class_create(THIS_MODULE, EEP_CLASS);
    
    /* Each eeprom bank represented as a char device (cdev) */
    for (i = 0; i < EEP_NBANK; i++) 
    {
        /* Tie file_operations to the cdev */
        cdev_init(&my_cdev[i], &eep_fops);
        eep_cdev[i].owner = THIS_MODULE;
        
        /* Device number to use to add cdev to the core */
        curr_dev = MKDEV(MAJOR(dev_num), MINOR(dev_num) + i);
        
        /* Now make the device live for the users to access */
        cdev_add(&eep_cdev[i], curr_dev, 1);
        
        /* create a device node each device /dev/eep-mem0, /dev/eep-mem1,
        * With our class used here, devices can also be viewed under
        * /sys/class/eep-class.
        */
        device_create(eep_class,
                      NULL,
                      /* no parent device */
                      curr_dev,
                      NULL,
                      /* no additional data */
                      EEP_DEVICE_NAME "%d", i); /* eep-mem[0-7] */
    }
    return 0;
}