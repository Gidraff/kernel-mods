#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "my_device"
#define CLASS_NAME "my_device_class"

static dev_t my_device_number;
static struct class* my_device_class = NULL;
static struct cdev my_cdev;

static char* msg_buffer = NULL;
static int buffer_size = 1024;

// Forward declarations of device operations
static int device_open(struct inode*, struct file*);
static int device_release(struct inode*, struct file*);
static ssize_t device_read(struct file*, char __user*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char __user*, size_t, loff_t*);

// File operations structure
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = device_open,
    .release = device_release,
    .read = device_read,
    .write = device_write,
};

static int __init device_init(void) {
    int result;

    printk(KERN_INFO "my_device: Initializing the device module.\n");

    // Allocate a major number dynamically
    result = alloc_chrdev_region(&my_device_number, 0, 1, DEVICE_NAME);
    if (result < 0) {
        printk(KERN_ALERT "my_device: Failed to allocate a major number. Result: %d\n", result);
        return result;
    }
    printk(KERN_INFO "my_device: Allocated major number %d.\n", MAJOR(my_device_number));

    // Initialize the cdev structure
    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    // Add the character device to the system
    result = cdev_add(&my_cdev, my_device_number, 1);
    if (result < 0) {
        printk(KERN_ALERT "my_device: Failed to add character device. Result: %d\n", result);
        unregister_chrdev_region(my_device_number, 1);
        return result;
    }
    printk(KERN_INFO "my_device: Character device added successfully.\n");

    // Create a device class
    // NOTE: The class_create function no longer takes the THIS_MODULE parameter
    my_device_class = class_create(CLASS_NAME);
    if (IS_ERR(my_device_class)) {
        printk(KERN_ALERT "my_device: Failed to create device class.\n");
        cdev_del(&my_cdev);
        unregister_chrdev_region(my_device_number, 1);
        return PTR_ERR(my_device_class);
    }
    printk(KERN_INFO "my_device: Device class created successfully.\n");

    // Create a device node in sysfs
    if (IS_ERR(device_create(my_device_class, NULL, my_device_number, NULL, DEVICE_NAME))) {
        printk(KERN_ALERT "my_device: Failed to create device.\n");
        class_destroy(my_device_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(my_device_number, 1);
        return -1;
    }
    printk(KERN_INFO "my_device: Device node created in /dev/%s.\n", DEVICE_NAME);

    // Allocate buffer for communication
    msg_buffer = kmalloc(buffer_size, GFP_KERNEL);
    if (!msg_buffer) {
        printk(KERN_ALERT "my_device: Failed to allocate memory for the buffer.\n");
        device_destroy(my_device_class, my_device_number);
        class_destroy(my_device_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(my_device_number, 1);
        return -ENOMEM;
    }
    memset(msg_buffer, 0, buffer_size);

    return 0;
}

static void __exit device_exit(void) {
    kfree(msg_buffer);
    device_destroy(my_device_class, my_device_number);
    class_destroy(my_device_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_device_number, 1);
    printk(KERN_INFO "my_device: Exited successfully.\n");
}

static int device_open(struct inode* inode, struct file* file) {
    printk(KERN_INFO "my_device: Device opened.\n");
    return 0;
}

static int device_release(struct inode* inode, struct file* file) {
    printk(KERN_INFO "my_device: Device closed.\n");
    return 0;
}

static ssize_t device_read(struct file* file, char __user* buf, size_t count, loff_t* pos) {
    ssize_t bytes_to_read = buffer_size - *pos;
    if (bytes_to_read > count) {
        bytes_to_read = count;
    }

    if (bytes_to_read < 0) {
        return 0;
    }
    
    if (copy_to_user(buf, msg_buffer + *pos, bytes_to_read)) {
        return -EFAULT;
    }
    
    *pos += bytes_to_read;
    return bytes_to_read;
}

static ssize_t device_write(struct file* file, const char __user* buf, size_t count, loff_t* pos) {
    ssize_t bytes_to_write = buffer_size - *pos;
    if (bytes_to_write > count) {
        bytes_to_write = count;
    }

    if (copy_from_user(msg_buffer + *pos, buf, bytes_to_write)) {
        return -EFAULT;
    }

    // Null-terminate the buffer if a full line is written
    if (*pos + bytes_to_write < buffer_size) {
        msg_buffer[*pos + bytes_to_write] = '\0';
    } else {
        msg_buffer[buffer_size - 1] = '\0';
    }

    *pos += bytes_to_write;
    return bytes_to_write;
}

module_init(device_init);
module_exit(device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gemini");
MODULE_DESCRIPTION("A simple character device for kernel-to-Go communication");
MODULE_VERSION("1.0.0");

