#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <asm/uaccess.h> 
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/gfp.h>

#define MAX_MESSAGE_LENGTH 90
#define BUF_SIZE 64
#define PACKET_TEMPLATE "Packet! %s %s -----> %s\n"
#define PACKET_TEMPLATE_PORT "Packet! %s %s:%i -----> %s:%i\n"

static char * message;
static int count;
static char no_packages;

struct iphdr * packet_info;
struct tcphdr *tcp_head;
struct udphdr *udp_head;
char packet_string[MAX_MESSAGE_LENGTH];
char dst_ip[16];
char src_ip[16];
char protocol[10];

struct circBuf_t {
    char buffer[BUF_SIZE][MAX_MESSAGE_LENGTH];
    unsigned int head;
    unsigned int tail;
    unsigned int size;
};

struct circBuf_t cbuf;

static dev_t dev;
static struct cdev c_dev;
static struct class * cl;

static int my_open(struct inode *inode, struct file *file);
static int my_close(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *filp, char *buffer,
					   size_t length, loff_t * offset);
static ssize_t my_write(struct file *filp, const char *buff,
						size_t len, loff_t * off);

// https://embeddedartistry.com/blog/2017/4/6/circular-buffers-in-cc
int circular_buf_reset(struct circBuf_t * cbuf)
{
	// очищает буфер, обнуляя head и tail
    int r;
	r = -1;

    if (cbuf)
    {
        cbuf->head = 0;
        cbuf->tail = 0;
        r = 0;
    }

    return r;
}

int circular_buf_empty(struct circBuf_t cbuf)
{
    return (cbuf.head == cbuf.tail);
}

int circular_buf_put(struct circBuf_t * cbuf, char * data)
{
    int r;
	r = -1;

    if (cbuf)
    {
        strcpy(cbuf->buffer[cbuf->head], data);

        cbuf->head = (cbuf->head + 1) % cbuf->size;

        if (cbuf->head == cbuf->tail)
        {
            cbuf->tail = (cbuf->tail + 1) % cbuf->size;
        }

        r = 0;
    }

    return r;
}

int circular_buf_get(struct circBuf_t * cbuf, char ** data)
{
    int r;
	r = -1;

    if (cbuf && data && !circular_buf_empty(*cbuf))
    {
		strcpy(*data, cbuf->buffer[cbuf->tail]);
        cbuf->tail = (cbuf->tail + 1) % cbuf->size;

        r = 0;
    }

    return r;
}

static struct file_operations hello_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
	.read = my_read,
	.write = my_write
};

/* Структура, которая используется для регистрации обработчика. */
static struct nf_hook_ops nfho;

/* Собственно функция обработчика. */
unsigned int hook_func(const struct nf_hook_ops *ops,
					   struct sk_buff *skb,
					   const struct net_device *in,
					   const struct net_device *out,
					   int (*okfn)(struct sk_buff *))
{
	packet_info = ip_hdr(skb);

	snprintf(src_ip, 16, "%pI4", &packet_info->saddr);
	snprintf(dst_ip, 16, "%pI4", &packet_info->daddr);

	if (packet_info->protocol == IPPROTO_ICMP) {
		strcpy(protocol, "ICMP");
		sprintf(packet_string, PACKET_TEMPLATE, protocol, src_ip, dst_ip);
	}
	else if (packet_info->protocol == IPPROTO_IGMP) {
		strcpy(protocol, "IGMP");
		sprintf(packet_string, PACKET_TEMPLATE, protocol, src_ip, dst_ip);
	}
	else if (packet_info->protocol == IPPROTO_TCP) {
		strcpy(protocol, "TCP");
		/* tcp_head = tcp_hdr(skb);
		 * (struct tcphdr *)(skb->data + (ip_hdr(skb)->ihl * 4));
		 */
		tcp_head = (struct tcphdr *)(skb->data + (ip_hdr(skb)->ihl * 4));
		sprintf(packet_string, PACKET_TEMPLATE_PORT, protocol, src_ip,
				htons(tcp_head->source), dst_ip, htons(tcp_head->dest));
	}
	else if (packet_info->protocol == IPPROTO_UDP) {
		strcpy(protocol, "UDP");
		/* udp_head = udp_hdr(skb);
		 * (struct udphdr *)(skb->data + (ip_hdr(skb)->ihl * 4));
		 */
		udp_head = (struct udphdr *)(skb->data + (ip_hdr(skb)->ihl * 4));
		sprintf(packet_string, PACKET_TEMPLATE_PORT, protocol, src_ip,
			    htons(udp_head->source), dst_ip, htons(udp_head->dest));
	}
	else {
		strcpy(protocol, "none");
		sprintf(packet_string, PACKET_TEMPLATE, protocol, src_ip, dst_ip);
	}

	printk(packet_string);
	circular_buf_put(&cbuf, packet_string);

    return NF_ACCEPT;          /* NF_DROP отбрасывать все пакеты */
}

/* Процедура регистрирует обработчик сетевых пакетов */
int ip_handler_register(void)
{
	/* Fill in our hook structure */
	nfho.hook = hook_func;
	/* Handler function */
	nfho.hooknum = NF_INET_PRE_ROUTING; /* Первое сетевое событие для IPv4 */
	nfho.pf = PF_INET;
	nfho.priority = NF_IP_PRI_FIRST;/* Назначаем обработчику 1-ый приоритет */

	nf_register_hook(&nfho);
	printk("Hello: Netfilter hook registered!\n");
	return 0;
}

/* Процедура отменяет регистрацию обработчика сетевых пакетов */
void ip_handler_unregister(void)
{
	nf_unregister_hook(&nfho);
	printk("Hello: Netfilter hook unregistered!\n");
}

static int __init hello_init(void) /* Инициализация */
{
	int retval;
	bool allocated = false;
	bool created = false;
	cl = NULL;

	retval = alloc_chrdev_region(&dev, 0, 1, "hello");
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

	printk(KERN_INFO "Hello: registered");
	return 0;

err:
	printk("Hello: initialization failed with code %08x\n", retval);

	if (created)
		device_destroy(cl, dev);

	if (allocated)
		unregister_chrdev_region(dev, 1);

	if (cl)
		class_destroy(cl);

	return retval;
}

static int my_open(struct inode *inode, struct file *file)
{
	ip_handler_register();

	cbuf.size = BUF_SIZE;
	circular_buf_reset(&cbuf);
	
	count = -1;
	no_packages = '\0';
	
	message = (char *)kmalloc(MAX_MESSAGE_LENGTH, GFP_USER);

	return 0;
}

static int my_close(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t my_read(struct file *filp,  
					   char *buffer, /* буфер данных */
					   size_t length, /* длина буфера */
					   loff_t * offset)
{
start:

	if (count == -1) {
		if (circular_buf_empty(cbuf)) {
			copy_to_user(buffer, &no_packages, sizeof(char));
			return sizeof(char);
		}
		else
			circular_buf_get(&cbuf, &message);
		count++;
	}

	if (count <= MAX_MESSAGE_LENGTH) {
		if (message[count] == 0) {
			count = -1;
			goto start;
		}

		if (copy_to_user(buffer, &message[count], sizeof(char)))
			return -EFAULT;
		else {
			count++;
			return sizeof(char);
		}
	}
	count = -1;
	goto start;

	return 0;
}

static ssize_t my_write(struct file *filp, const char *buff,
						size_t len, loff_t * off)
{
	//#define EINVAL          22      /* Invalid argument */
    return -EINVAL;
}


static void __exit hello_exit(void) /* Деинициализаия */
{
    printk(KERN_INFO "Hello: unregistered\n");
    device_destroy (cl, dev);
    unregister_chrdev_region (dev, 1);
    class_destroy (cl);
	ip_handler_unregister();
	kfree(message);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Sidyakin");
MODULE_DESCRIPTION("Simple loadable kernel module");