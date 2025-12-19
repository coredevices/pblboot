/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include "firmware.h"

#include <errno.h>
#include <inttypes.h>
#include <stdint.h>

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/flash.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/crc.h>
#include <zephyr/sys/util.h>

#include <pb/bootbit.h>
#include <pb/fwjump.h>

LOG_MODULE_DECLARE(pblboot, CONFIG_PBLBOOT_LOG_LEVEL);

#define SLOT0_ADDR DT_REG_ADDR(DT_CHOSEN(pb_slot0))
#define SLOT1_ADDR DT_REG_ADDR(DT_CHOSEN(pb_slot1))
#define PRF_ADDR   DT_REG_ADDR(DT_CHOSEN(pb_prf))

#define PBLBOOT_MAGIC 0x96f3b83dUL

struct firmware_header {
	uint32_t magic;
	uint32_t header_length;
	uint64_t timestamp;
	uint32_t start_offset;
	uint32_t length;
	uint32_t crc;
} __packed;

static uint8_t buf[CONFIG_PB_FLASH_READ_BUF_SIZE];
static const struct device *flash = DEVICE_DT_GET(DT_CHOSEN(zephyr_flash_controller));

static int firmware_header_get(uint32_t address, struct firmware_header *hdr)
{
	for (unsigned int retry = 0; retry < 5; retry++) {
		int ret;

		LOG_INF("Fetch header (retry=%d)", retry);

		ret = flash_read(flash, address, hdr, sizeof(*hdr));
		if (ret < 0) {
			LOG_ERR("Failed to read from flash (err %d)", ret);
			return ret;
		}

		for (size_t i = 0; i < sizeof(*hdr); i++) {
			LOG_INF("%02x", ((uint8_t *)hdr)[i]);
		}

		if (hdr->magic != PBLBOOT_MAGIC) {
			LOG_ERR("Invalid firmware header magic");
			continue;
		}

		if (hdr->header_length != sizeof(*hdr)) {
			LOG_ERR("Invalid firmware header length");
			continue;
		}

		LOG_INF("Firmware header: timestamp=%" PRIu64 ", start_offset=0x%" PRIx32
			", length=0x%" PRIx32 ", crc=0x%" PRIx32,
			hdr->timestamp, hdr->start_offset, hdr->length, hdr->crc);

		return 0;
	}

	return -EINVAL;
}

static int firmware_validate(uint32_t address, const struct firmware_header *hdr)
{
	int ret;
	uint32_t pending;
	uint32_t addr;
	uint32_t crc;

	crc = crc32_ieee(NULL, 0U);
	pending = hdr->length;
	addr = address + hdr->start_offset;
	while (pending > 0U) {
		size_t len = MIN(sizeof(buf), pending);
		ret = flash_read(flash, addr, buf, len);
		if (ret < 0) {
			LOG_ERR("Failed to read from flash (err %d)", ret);
			return ret;
		}

		crc = crc32_ieee_update(crc, buf, len);

		pending -= len;
		addr += len;
	}

	if (crc != hdr->crc) {
		LOG_ERR("Firmware CRC mismatch (calculated 0x%08x, expected 0x%08x)",
			crc, hdr->crc);
		return -EIO;
	}

	return 0;
}

int pb_firmware_init(void)
{
	if (!device_is_ready(flash)) {
		LOG_ERR("Flash device not ready");
		return -ENODEV;
	}

	return 0;
}

int pb_firmware_load_prf(void)
{
	uint32_t prf_load_address;
	struct firmware_header hdr;
	int ret;

	ret = firmware_header_get(PRF_ADDR, &hdr);
	if (ret < 0) {
		LOG_ERR("PRF not found or invalid (err %d)", ret);
		return ret;
	}

	ret = firmware_validate(PRF_ADDR, &hdr);
	if (ret < 0) {
		LOG_ERR("PRF image is corrupted");
		return ret;
	}

	pb_bootbit_prf_starting_set();

	prf_load_address = CONFIG_FLASH_BASE_ADDRESS + PRF_ADDR + hdr.start_offset;
	LOG_INF("Loading PRF at address 0x%" PRIx32, prf_load_address);
	pb_fwjump(prf_load_address);

	return 0;
}

int pb_firmware_load(void)
{
	struct firmware_header slot0_hdr;
	struct firmware_header slot1_hdr;
	uint32_t slot0_load_address = 0U;
	uint32_t slot1_load_address = 0U;
	bool slot0_valid;
	bool slot1_valid;
	int ret;

	slot0_valid = false;
	ret = firmware_header_get(SLOT0_ADDR, &slot0_hdr);
	if (ret == 0) {
		ret = firmware_validate(SLOT0_ADDR, &slot0_hdr);
		if (ret == 0) {
			slot0_valid = true;
			slot0_load_address =
				CONFIG_FLASH_BASE_ADDRESS + SLOT0_ADDR + slot0_hdr.start_offset;
			LOG_INF("slot0 firmware valid (0x%" PRIx32 ", %" PRIu64 ")",
				slot0_load_address, slot0_hdr.timestamp);
		}
	}

	slot1_valid = false;
	ret = firmware_header_get(SLOT1_ADDR, &slot1_hdr);
	if (ret == 0) {
		ret = firmware_validate(SLOT1_ADDR, &slot1_hdr);
		if (ret == 0) {
			slot1_valid = true;
			slot1_load_address =
				CONFIG_FLASH_BASE_ADDRESS + SLOT1_ADDR + slot1_hdr.start_offset;
			LOG_INF("slot1 firmware valid (0x%" PRIx32 ", %" PRIu64 ")",
				slot1_load_address, slot1_hdr.timestamp);
		}
	}

	if (slot0_valid) {
		if (slot1_valid && (slot1_hdr.timestamp > slot0_hdr.timestamp)) {
			LOG_INF("Loading slot1 firmware @ 0x%" PRIx32, slot1_load_address);
			pb_fwjump(slot1_load_address);
		} else {
			LOG_INF("Loading slot0 firmware @ 0x%" PRIx32, slot0_load_address);
			pb_fwjump(slot0_load_address);
		}
	} else if (slot1_valid) {
		if (slot0_valid && (slot0_hdr.timestamp > slot1_hdr.timestamp)) {
			LOG_INF("Loading slot0 firmware @ 0x%" PRIx32, slot0_load_address);
			pb_fwjump(slot0_load_address);
		} else {
			LOG_INF("Loading slot1 firmware @ 0x%" PRIx32, slot1_load_address);
			pb_fwjump(slot1_load_address);
		}
	} else {
		LOG_ERR("No valid firmware image");
		return pb_firmware_load_prf();
	}
}
