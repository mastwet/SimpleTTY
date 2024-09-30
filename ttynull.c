// SPDX-License-Identifier: GPL-2.0
/*
 * NULL TTY driver with printk and debugging
 */

#include <linux/console.h>
#include <linux/module.h>
#include <linux/tty.h>
#include <linux/kernel.h>  // For printk
#include <linux/uaccess.h> // For copy_from_user if needed

static const struct tty_port_operations ttynull_port_ops;
static struct tty_driver *ttynull_driver;
static struct tty_port ttynull_port;

// 打开设备
static int ttynull_open(struct tty_struct *tty, struct file *filp)
{
    printk(KERN_INFO "ttynull: Open called\n");
    return tty_port_open(&ttynull_port, tty, filp);
}

// 关闭设备
static void ttynull_close(struct tty_struct *tty, struct file *filp)
{
    printk(KERN_INFO "ttynull: Close called\n");
    tty_port_close(&ttynull_port, tty, filp);
}

// 挂断设备
static void ttynull_hangup(struct tty_struct *tty)
{
    printk(KERN_INFO "ttynull: Hangup called\n");
    tty_port_hangup(&ttynull_port);
}

// 写数据到设备
static int ttynull_write(struct tty_struct *tty, const unsigned char *buf, int count)
{
    char message[256];
    int len = min(count, (int)(sizeof(message) - 1));

    printk(KERN_INFO "ttynull: Write called with count=%d\n", count);

    if (len <= 0) {
        printk(KERN_WARNING "ttynull: Invalid write length\n");
        return -EINVAL;
    }

    memcpy(message, buf, len);
    message[len] = '\0';

    printk(KERN_INFO "ttynull: Received message: %s\n", message);

    return count;
}

// 查询可写入空间
static int ttynull_write_room(struct tty_struct *tty)
{
    printk(KERN_INFO "ttynull: write_room called\n");
    return 65536;
}

static const struct tty_operations ttynull_ops = {
    .open = ttynull_open,
    .close = ttynull_close,
    .hangup = ttynull_hangup,
    .write = ttynull_write,
    .write_room = ttynull_write_room,
};

static struct tty_driver *ttynull_device(struct console *c, int *index)
{
    *index = 0;
    printk(KERN_INFO "ttynull: ttynull_device called\n");
    return ttynull_driver;
}

static struct console ttynull_console = {
    .name = "ttynull",
    .device = ttynull_device,
    .flags = CON_PRINTBUFFER, // 确保缓冲区被打印
};

static int __init ttynull_init(void)
{
    struct tty_driver *driver;
    int ret;

    printk(KERN_INFO "ttynull: Initializing module\n");

    driver = tty_alloc_driver(1,
        TTY_DRIVER_RESET_TERMIOS |
        TTY_DRIVER_REAL_RAW |
        TTY_DRIVER_UNNUMBERED_NODE);
    if (IS_ERR(driver)) {
        printk(KERN_ERR "ttynull: tty_alloc_driver failed\n");
        return PTR_ERR(driver);
    }

    tty_port_init(&ttynull_port);
    ttynull_port.ops = &ttynull_port_ops;

    driver->driver_name = "ttynull";
    driver->name = "ttynull";
    driver->type = TTY_DRIVER_TYPE_CONSOLE;
    driver->init_termios = tty_std_termios;
    driver->init_termios.c_oflag = OPOST | OCRNL | ONOCR | ONLRET;
    tty_set_operations(driver, &ttynull_ops);
    tty_port_link_device(&ttynull_port, driver, 0);

    ret = tty_register_driver(driver);
    if (ret < 0) {
        printk(KERN_ERR "ttynull: tty_register_driver failed\n");
        tty_driver_kref_put(driver);
        tty_port_destroy(&ttynull_port);
        return ret;
    }

    ttynull_driver = driver;
    register_console(&ttynull_console);

    printk(KERN_INFO "ttynull: Module loaded with major=%d\n", ttynull_driver->major);

    return 0;
}

static void __exit ttynull_exit(void)
{
    printk(KERN_INFO "ttynull: Unloading module\n");
    unregister_console(&ttynull_console);
    tty_unregister_driver(ttynull_driver);
    tty_driver_kref_put(ttynull_driver);
    tty_port_destroy(&ttynull_port);

    printk(KERN_INFO "ttynull: Module unloaded\n");
}

module_init(ttynull_init);
module_exit(ttynull_exit);

MODULE_DESCRIPTION("NULL TTY driver with printk and debugging");
MODULE_LICENSE("GPL v2");
