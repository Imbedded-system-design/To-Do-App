#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/ioctl.h>

#define DRIVER_AUTHOR "CAUSW jihun jeon"
#define DRIVER_DESC "driver for pushbutton"

#define PBTN_NAME "pushbutton"
#define PBTN_MODULE_VERSION "pushbutton V1.0"
#define PBTN_ADDR 0x50

// gpio fpga interface provided
extern ssize_t iom_fpga_itf_read(unsigned int addr);
extern ssize_t iom_fpga_itf_write(unsigned int addr, unsigned short int value);

// global
static int pbutton_in_use = 0;

int pbtn_open(struct inode *pinode, struct file *pfile)
{
	if (pbutton_in_use != 0)
	{
		return -EBUSY;
	}

	pbutton_in_use = 1;

	return 0;
}

int pbtn_release(struct inode *pinode, struct file *pfile)
{
	pbutton_in_use = 0;

	return 0;
}

ssize_t pbtn_read(struct file *pinode, char *gdata, size_t len, loff_t *off_what)
{
	int i;
	unsigned char bytevalues[9];
	unsigned short wordvalue;
	char *tmp = NULL;

	tmp = gdata;

	for (i = 0; i < 9; i++)
	{
		wordvalue = iom_fpga_itf_read((unsigned int)PBTN_ADDR + (i * 2));
		bytevalues[i] = (unsigned char)wordvalue & 0xFF;
	}

	if (copy_to_user(tmp, &bytevalues, 9))
	{
		return -EFAULT;
	}

	return len;
}

static struct file_operations pbtn_fops = {
	.owner = THIS_MODULE,
	.open = pbtn_open,
	.read = pbtn_read,
	.release = pbtn_release,
};

static struct miscdevice pbtn_driver = {
	.fops = &pbtn_fops,
	.name = PBTN_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};

int pbtn_init(void)
{
	misc_register(&pbtn_driver);
	printk(KERN_INFO "driver: %s driver init\n", PBTN_NAME);

	return 0;
}

void pbtn_exit(void)
{
	misc_deregister(&pbtn_driver);
	printk(KERN_INFO "driver: %s driver exit\n", PBTN_NAME);
}

module_init(pbtn_init);
module_exit(pbtn_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
