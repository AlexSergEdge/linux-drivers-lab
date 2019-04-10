#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#define HELLO_MAJOR 250






/* define name of the drivers */
#define FILENAME "/dev/hello"

// Эмуляция порта
#define PORT_EMULATION

#ifdef PORT_EMULATION
#define inb(port) prn_data_em
#define outb(data, port) prn_data_em = ((unsigned char)data);
unsigned char prn_data_em;
#endif


#define START_MINOR_CODE 0
#define ALLOC_MINOR 1
#define UNREG_DEV 1
static dev_t dev;
static struct cdev c_dev;
static struct class * cl;

// Макроопределение для кода команды
#define LEDCMD_RESET_STATE _IO(HELLO_MAJOR, 1) /* погасить все индикаторы */
#define LEDCMD_GET_STATE _IOR(HELLO_MAJOR, 2, unsigned char *) /* вернуть состояния всех индикаторов */
#define LEDCMD_GET_LED_STATE _IOWR(HELLO_MAJOR, 3,  led_t *) /* вернуть значение индикатора с указанным номером */
#define LEDCMD_SET_LED_STATE _IOW(HELLO_MAJOR, 4, led_t *) /* установить значение индикатора с указанным номером */ 

// Структура для управления отдельным диодом
typedef struct led {
    int pin;
    unsigned char value;
} led_t;



#define PARALLEL_PORT 0x378
/* Начальное состояние светодиодов */
#define INITIAL_STATE 0x00

static long hello_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    int retval = -EINVAL;
    led_t led;
    // байт порта
    unsigned char port_state_byte;

    //char bit_pos = 0b11111111;
    //int ret;


    /* Обработка команды чтения состояния светодиода */
    switch (cmd) {
        // Получение состояния 
        case LEDCMD_GET_LED_STATE:
            if (copy_from_user(&led, (led_t *)arg, sizeof(led_t))) /* копируем параметры из пользовательского буфера в локальную переменную */
                retval = -EACCES;           
            else if ((led.pin < 0) || (led.pin > 7)) /* Номер светодиода должен быть в диапазоне от 0 до 7 */
                retval = -EINVAL;
            else {
                led.value = (inb(PARALLEL_PORT) >> led.pin) & 1; /* Считываем состояния разрядов параллельного порта и сдвигаем    */
                if(copy_to_user((led_t *)arg, &led, sizeof(led_t))) /*  вправо на led.pin разрядов и маскируем 1. Считаем, что светодиод 0 подключен */
                    retval = -EACCES;                               /* к младшему разряду порта.  Затем копируем всю структуру обратно в пользовательский буфер */
                else
                    retval = 0;
            }
            break;

        // Записываем в порт 0    
        case LEDCMD_RESET_STATE:
            outb(INITIAL_STATE, PARALLEL_PORT);
            retval = 0;
            break;

        // Копируем байт из порта в выходной буфер
        case LEDCMD_GET_STATE:
            port_state_byte = inb(PARALLEL_PORT);
            if(copy_to_user((unsigned char *)arg, &port_state_byte, sizeof(port_state_byte)))
                retval = -EACCES;
            else
                retval = 0;
            break;

        // Устанавливаем/сбрасывем состояние порта
        case LEDCMD_SET_LED_STATE:
            // Получаем содержимое порта
            if (copy_from_user(&led, (led_t *)arg, sizeof(led_t)))
                retval = -EACCES;
            else
                // Если содержимое в пределахот 0 до 8
                if ((led.pin < 0) || (led.pin > 7))
                    retval = -EINVAL;
                else {
                    // Получаем состояние порта
                    port_state_byte = inb(PARALLEL_PORT);
                    // Переключаем бит
                    port_state_byte ^= 1UL << led.pin;
                    outb(port_state_byte, PARALLEL_PORT);
                    retval = 0;
                }
            break;

        default:
            printk("Bad ioctl command!\n");
            break;
    }

    return retval;
}

static int hello_open(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t hello_read(struct file *filp, char *buffer,
                       size_t length, loff_t * offset) {
    return 0;
}

static ssize_t hello_write(struct file *filp, const char *buff,
                        size_t len, loff_t * off) {
        return -EINVAL;
}

static int hello_close(struct inode *inode, struct file *file) {
    return 0;
}


//
static struct file_operations hello_fops = {
    .owner = THIS_MODULE,
    .open = hello_open,
    .release = hello_close,
    .read = hello_read,
    .write = hello_write,
    .unlocked_ioctl = hello_ioctl
};

static int __init hello_init(void) {
    int retval;
    bool allocated = false;
    bool created = false;
    cl = NULL;

    retval = alloc_chrdev_region(&dev, START_MINOR_CODE,
                                 ALLOC_MINOR, "hello");
    if (retval)
        goto err;

    allocated = true;
    printk(KERN_INFO "Major number = %d Minor number = %d\n",
            MAJOR(dev), MINOR(dev));
    cl = class_create(THIS_MODULE, "teach_devices");
    if (!cl) {
        retval = -1;
        goto err;
    }

    if (device_create(cl, NULL, dev, NULL, "hello") == NULL) {
        retval = -1;
        goto err;
    }

    created = true;
    cdev_init(&c_dev, &hello_fops);
    retval = cdev_add(&c_dev, dev, 1);
    if (retval)
        goto err;

    printk(KERN_INFO "Hello: regisered\n");
    return 0;

err:
    printk(KERN_INFO "Hello: initialization failed with code %d\n",
           retval);
    if (created)
        device_destroy(cl, dev);

    if (allocated)
        unregister_chrdev_region(dev, UNREG_DEV);

    if (cl)
        class_destroy(cl);

    return retval;
}


static void __exit hello_exit(void) {
    printk(KERN_INFO "Hello: unregistered\n");
    device_destroy (cl, dev);
    unregister_chrdev_region (dev, UNREG_DEV);
    class_destroy (cl);
}





module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Brink Alexander");
MODULE_DESCRIPTION("Simple loadable kernel module");
