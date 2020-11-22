#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <asm/ioctl.h>
#include <linux/delay.h>

#define DRIVER_AUTHOR	"CAUSW Jihun Jeon"
#define DRIVER_DESC		"driver for dotmatrix2"

#define DOTM_NAME		"matrix"
#define DOTM_MODULE_VERSION	"matrix V1.0"
#define DOTM_ADDR		0x210

#define DOTM_MAGIC		0xBC
#define DOTM_SET_ALL	_IOW(DOTM_MAGIC, 0, int)
#define DOTM_SET_CLEAR	_IOW(DOTM_MAGIC, 1, int)

// Android Definition
#define DOT_EMPTY				0
#define DOT_FILTER_ALL			1
#define DOT_FILTER_ACTIVE		2
#define DOT_FILTER_COMPLETE		3

extern ssize_t iom_fpga_itf_read(unsigned int addr);
extern ssize_t iom_fpga_itf_write(unsigned int addr, unsigned short int value);

static int dotm_in_use = 0;

unsigned char dotm_fontmap_decimal[10][10] = {
	{0x3e, 0x7f, 0x63, 0x73, 0x73, 0x6f, 0x67, 0x63, 0x7f, 0x3e},	// 0
	{0x0c, 0x1c, 0x1c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x1e},	// 1
	{0x7e, 0x7f, 0x03, 0x03, 0x3f, 0x7e, 0x60, 0x60, 0x7f, 0x7f},	// 2
	{0xfe, 0x7f, 0x03, 0x03, 0x7f, 0x7f, 0x03, 0x03, 0x7f, 0x7e},	// 3
	{0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7f, 0x7f, 0x06, 0x06},	// 4
	{0x7f, 0x7f, 0x60, 0x60, 0x7e, 0x7f, 0x03, 0x03, 0x7f, 0x7e},	// 5
	{0x60, 0x60, 0x60, 0x60, 0x7e, 0x7f, 0x63, 0x63, 0x7f, 0x3e},	// 6
	{0x7f, 0x7f, 0x63, 0x63, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03},	// 7
	{0x3e, 0x7f, 0x63, 0x63, 0x7f, 0x7f, 0x63, 0x63, 0x7f, 0x3e},	// 8
	{0x3e, 0x7f, 0x63, 0x63, 0x7f, 0x3f, 0x03, 0x03, 0x03, 0x03}	// 9
};

unsigned char dotm_fontmap_full[10] = {
	0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f
};

unsigned char dotm_fontmap_empty[10] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// Filter_All
unsigned char dotm_filter_all[10] = {
	0x00, 0x7f, 0x55, 0x7f, 0x55, 0x7f, 0x55, 0x7f, 0x00, 0x00
};
// Filter_Active
unsigned char dotm_filter_active[10] = {
	0x0c, 0x12, 0x12, 0x0c, 0x39, 0x4e, 0x08, 0x0c, 0x52, 0x22
};
// Filter_Complete
unsigned char dotm_filter_complete[10] = {
	0x00, 0x7e, 0x41, 0x42, 0x54, 0x4a, 0x42, 0x7e, 0x00, 0x00
};

int dotm_open(struct inode *pinode, struct file *pfile){
	if(dotm_in_use != 0){
		return -EBUSY;
	}

	dotm_in_use = 1;

	return 0;
}

int dotm_release(struct inode *pinode, struct file *pfile){
	dotm_in_use = 0;

	return 0;
}

ssize_t dotm_write(struct file *pinode, const char *gdata, size_t len, loff_t *off_what)
{
	int ret, i;
	unsigned char num;
	unsigned short wordvalue;
	const char *tmp = NULL;

	tmp = gdata;

	if (len > 1){
		printk(KERN_WARNING "only 1 byte data will be processed\n");
		len = 1;
	}

	ret = copy_from_user(&num, tmp, 1);
	if (ret < 0){
		return -EFAULT;
	}

	// if-else
	if(num == DOT_EMPTY){
		for(i = 0; i < 10; i++){
			wordvalue = dotm_fontmap_empty[i] & 0x7F;
			iom_fpga_itf_write((unsigned int)DOTM_ADDR + (i*2), wordvalue);
		}
	}
	else if(num == DOT_FILTER_ALL){
		for(i = 0; i < 10; i++){
			wordvalue = dotm_filter_all[i] & 0x7F;
			iom_fpga_itf_write((unsigned int)DOTM_ADDR + (i*2), wordvalue);
		}
	}
	else if(num == DOT_FILTER_ACTIVE){
		for(i = 0; i < 10; i++){
			wordvalue = dotm_filter_active[i] & 0x7F;
			iom_fpga_itf_write((unsigned int)DOTM_ADDR + (i*2), wordvalue);
		}
	}
	else if(num == DOT_FILTER_COMPLETE){
		for(i = 0; i < 10; i++){
			wordvalue = dotm_filter_complete[i] & 0x7F;
			iom_fpga_itf_write((unsigned int)DOTM_ADDR + (i*2), wordvalue);
		}
	}

	return len;
}

static long dotm_ioctl(struct file *pinode, unsigned int cmd, unsigned long data)
{
	int i;
	unsigned short wordvalue;

	switch(cmd){
	case DOTM_SET_ALL:
		for (i = 0; i < 10; i++){
			wordvalue = dotm_fontmap_full[i] & 0x7F;
			iom_fpga_itf_write((unsigned int)DOTM_ADDR+(i*2), wordvalue);
		}
		break;
	case DOTM_SET_CLEAR:
		for (i = 0; i < 10; i++){
			wordvalue = dotm_fontmap_empty[i] & 0x7F;
			iom_fpga_itf_write((unsigned int)DOTM_ADDR+(i*2), wordvalue);
		}
		break;
	}

	return 0;
}

static struct file_operations dotm_fops = {
	.owner	= THIS_MODULE,
	.open	= dotm_open,
	.write	= dotm_write,
	.unlocked_ioctl	= dotm_ioctl,
	.release = dotm_release,
};

static struct miscdevice dotm_driver = {
	.fops	= &dotm_fops,
	.name	= DOTM_NAME,
	.minor	= MISC_DYNAMIC_MINOR,
};

int dotm_init(void)
{
	misc_register(&dotm_driver);
	printk(KERN_INFO "driver: %s driver init\n", DOTM_NAME);

	return 0;
}

void dotm_exit(void)
{
	misc_deregister(&dotm_driver);
	printk(KERN_INFO "driver: %s driver exit\n", DOTM_NAME);
}

module_init(dotm_init);
module_exit(dotm_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");
