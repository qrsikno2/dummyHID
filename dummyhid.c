#include <linux/module.h>
#include <linux/hid.h>
#include <linux/device.h>
#include <linux/random.h>
#include <linux/sched.h>
#include <linux/usb.h>
#include <linux/wait.h>

#include "hid-ids.h"
#include "hid-lg.h"

#define LG_RDESC		0x001
#define LG_BAD_RELATIVE_KEYS	0x002
#define LG_DUPLICATE_USAGES	0x004
#define LG_EXPANDED_KEYMAP	0x010
#define LG_IGNORE_DOUBLED_WHEEL	0x020
#define LG_WIRELESS		0x040
#define LG_INVERT_HWHEEL	0x080
#define LG_NOGET		0x100
#define LG_FF			0x200
#define LG_FF2			0x400
#define LG_RDESC_REL_ABS	0x800
#define LG_FF3			0x1000
#define LG_FF4			0x2000

#define LG_ZJM		        0x4000	

/* Size of the original descriptors of the Driving Force (and Pro) wheels */
#define DF_RDESC_ORIG_SIZE	130
#define DFP_RDESC_ORIG_SIZE	97
#define FV_RDESC_ORIG_SIZE	130
#define MOMO_RDESC_ORIG_SIZE	87
#define MOMO2_RDESC_ORIG_SIZE	87
#define FFG_RDESC_ORIG_SIZE	85
#define FG_RDESC_ORIG_SIZE	82

#define LOGI_VENDOR_ID    0x046d    // Logitech VID
#define LOGI_MOUSE_PID    0xc542    // 你的鼠标 PID

// 设备匹配表
static const struct hid_device_id myhid_devices[] = {
    { HID_USB_DEVICE(LOGI_VENDOR_ID, LOGI_MOUSE_PID) },
    { } // 终止标记
};
MODULE_DEVICE_TABLE(hid, myhid_devices);

// 驱动初始化
static int myhid_probe(struct hid_device *hdev, const struct hid_device_id *id)
{
	struct usb_interface *iface;
	__u8 iface_num;
	unsigned int connect_mask = HID_CONNECT_DEFAULT;
	struct lg_drv_data *drv_data;
	int ret;

	ret = hid_parse(hdev);
	if (ret) {
		hid_err(hdev, "parse failed\n");
		goto err_free;
	}

	if (!hid_is_usb(hdev))
		return -EINVAL;
	
	iface = to_usb_interface(hdev->dev.parent);
	iface_num = iface->cur_altsetting->desc.bInterfaceNumber;
	printk(KERN_WARNING "hid-lg: Function %s called, param=hello world\n", __func__);


	drv_data = kzalloc(sizeof(struct lg_drv_data), GFP_KERNEL);
	if (!drv_data) {
		hid_err(hdev, "Insufficient memory, cannot allocate driver data\n");
		return -ENOMEM;
	}
	drv_data->quirks = id->driver_data;

	hid_set_drvdata(hdev, (void *)drv_data);

	if (drv_data->quirks & LG_NOGET)
		hdev->quirks |= HID_QUIRK_NOGET;



	ret = hid_hw_start(hdev, connect_mask);
	if (ret) {
		hid_err(hdev, "hw start failed\n");
		goto err_free;
	}


	printk(KERN_WARNING "hid-lg: Function %s finished, param=hello world\n", __func__);

	return 0;

err_stop:
	hid_hw_stop(hdev);
err_free:
	kfree(drv_data);
	return ret;
}

static void myhid_remove(struct hid_device *hdev)
{
    hid_hw_stop(hdev);
    printk(KERN_INFO "myhid: Device removed\n");
}

static int myhid_event(struct hid_device *hdev, struct hid_field *field,
		struct hid_usage *usage, __s32 value)
{
	struct lg_drv_data *drv_data = hid_get_drvdata(hdev);

	if ((drv_data->quirks & LG_INVERT_HWHEEL) && usage->code == REL_HWHEEL) {
		input_event(field->hidinput->input, usage->type, usage->code,
				-value);
		return 1;
	}

	return 0;
}

// 驱动结构体（保留原生 HID 行为）
static struct hid_driver myhid_driver = {
    .name = "myhid-logitech",
    .id_table = myhid_devices,
    .probe = myhid_probe,
    .remove = myhid_remove,
	.event = myhid_event,
};

module_hid_driver(myhid_driver); // 自动注册/注销驱动

MODULE_LICENSE("GPL");
MODULE_AUTHOR("myself");
MODULE_DESCRIPTION("Track Logitech HID Device Loading via printk");
