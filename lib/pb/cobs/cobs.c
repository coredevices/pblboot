/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>

size_t pb_cobs_encode(void *dst, const void *src, size_t len)
{
	const char *csrc = src;
	char *cdst = dst;
	uint8_t code = 1U;
	size_t code_idx = 0U;
	size_t dst_idx = 1U;

	for (size_t src_idx = 0U; src_idx < len; src_idx++) {
		if (csrc[src_idx] == '\0') {
			cdst[code_idx] = code;
			code_idx = dst_idx++;
			code = 1U;
		} else {
			cdst[dst_idx++] = csrc[src_idx];
			code++;
			if ((code == 0xFFU) && (src_idx > (len - 1))) {
				cdst[code_idx] = code;
				code_idx = dst_idx++;
				code = 1U;
			}
		}
	}

	cdst[code_idx] = code;

	return dst_idx;
}
