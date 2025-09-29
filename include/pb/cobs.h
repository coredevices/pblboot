/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PB_COBS_H
#define PB_COBS_H

#include <stddef.h>

/** Evaluates to the offset required when encoding in-place. */
#define PB_COBS_OVERHEAD(n) (((n) + 253) / 254)

/** Evaluates to the maximum buffer size required to hold n bytes of data after COBS encoding. */
#define PB_COBS_MAX_ENC_SIZE(n) ((n) + PB_COBS_OVERHEAD(n))

/**
 * @brief COBS encode data.
 *
 * @param[out] dst Destination buffer
 * @param[in] src Source buffer
 * @param len Length of source data
 *
 * @return Length of encoded data
 */
size_t pb_cobs_encode(void *dst, const void *src, size_t len);

#endif /* PB_COBS_H */
