/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#if defined(CONFIG_PRINTK) || defined(CONFIG_STDOUT_CONSOLE)

#include <string.h>

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/init.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/sys/crc.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/sys/libc-hooks.h>
#include <zephyr/sys/printk-hooks.h>
#include <zephyr/toolchain.h>

#include <pb/cobs.h>

#define MSG_BUF_LEN     256
#define MSG_HDR_LEN     35
#define FRAME_DELIMITER 0x55U

static const struct device *const dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));

static uint8_t msg_buf[MSG_BUF_LEN] = {
	/* Pulse transport push */
	0x50U,
	0x21U,
	/* Pulse protocol logging */
	0x00U,
	0x03U,
	/* Length (to be filled in) */
	0,
	0,
	/* Message type: text */
	1,
	/* Source filename */
	'P',
	'B',
	'L',
	'B',
	'O',
	'O',
	'T',
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* Log level and task */
	'*',
	'*',
	/* Timestamp */
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	/* Line number */
	0,
	0,
};

static size_t msg_len = MSG_HDR_LEN;

static int console_out(int c)
{
	if (c == '\n') {
		uint32_t crc;
		uint8_t msg_enc[PB_COBS_MAX_ENC_SIZE(msg_len + sizeof(crc))];
		size_t msg_enc_len;

		/* fill message length (not counting pulse transport push code)*/
		sys_put_be16(msg_len - 2U, &msg_buf[4]);

		/* append CRC32 */
		crc = crc32_ieee(msg_buf, msg_len);
		sys_put_le32(crc, &msg_buf[msg_len]);

		/* COBS encode */
		msg_enc_len = pb_cobs_encode(msg_enc, msg_buf, msg_len + sizeof(crc));

		/* send frame */
		uart_poll_out(dev, FRAME_DELIMITER);
		for (size_t i = 0U; i < msg_enc_len; i++) {
			if (msg_enc[i] == FRAME_DELIMITER) {
				uart_poll_out(dev, '\0');
			} else {
				uart_poll_out(dev, msg_enc[i]);
			}
		}
		uart_poll_out(dev, FRAME_DELIMITER);

		/* reset message length counter */
		msg_len = MSG_HDR_LEN;
	} else if (c != '\r' && msg_len < MSG_BUF_LEN) {
		msg_buf[msg_len++] = (uint8_t)c;
	}

	return c;
}

static int pulse_uart_console_init(void)
{
	if (!device_is_ready(dev)) {
		return -ENODEV;
	}

#ifdef CONFIG_STDOUT_CONSOLE
	__stdout_hook_install(console_out);
#endif
#ifdef CONFIG_PRINTK
	__printk_hook_install(console_out);
#endif
	return 0;
}

#ifdef CONFIG_EARLY_CONSOLE
SYS_INIT(pulse_uart_console_init, PRE_KERNEL_1, CONFIG_CONSOLE_INIT_PRIORITY);
#else
SYS_INIT(pulse_uart_console_init, POST_KERNEL, CONFIG_CONSOLE_INIT_PRIORITY);
#endif

#endif /* CONFIG_PRINTK || CONFIG_STDOUT_CONSOLE */
