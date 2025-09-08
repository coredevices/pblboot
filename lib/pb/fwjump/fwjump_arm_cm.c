/*
 * Copyright (c) 2025 Core Devices LLC
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/cache.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/sys/util.h>
#include <zephyr/toolchain.h>

#include <pb/fwjump.h>

#include <cmsis_core.h>

void FUNC_NORETURN pb_fwjump(uintptr_t addr)
{
#ifdef CONFIG_SYSTEM_TIMER_HAS_DISABLE_SUPPORT
	sys_clock_disable();
#endif

	/* disable IRQs and cleanup NVIC configuration */
	__disable_irq();

	for (uint8_t i = 0U; i < ARRAY_SIZE(NVIC->ICER); i++) {
		NVIC->ICER[i] = 0xFFFFFFFFUL;
	}

	for (uint8_t i = 0U; i < ARRAY_SIZE(NVIC->ICPR); i++) {
		NVIC->ICPR[i] = 0xFFFFFFFFUL;
	}

	/* flush and disable instruction/data caches */
	(void)sys_cache_instr_flush_all();
	(void)sys_cache_data_flush_all();
	sys_cache_instr_disable();
	sys_cache_data_disable();

#ifdef CONFIG_ARM_MPU
	/* cleanup MPU configurations */
	uint8_t num_regions = FIELD_GET(MPU_TYPE_DREGION_Msk, MPU->TYPE);

	for (uint8_t i = 0U; i < num_regions; i++) {
		ARM_MPU_ClrRegion(i);
	}
#endif /* CONFIG_ARM_MPU */

#if defined(CONFIG_BUILTIN_STACK_GUARD) && \
    defined(CONFIG_CPU_CORTEX_M_HAS_SPLIM)
	/* disable main/processor stack pointer limit addresses */
	__set_PSPLIM(0U);
	__set_MSPLIM(0U);
#endif /* CONFIG_BUILTIN_STACK_GUARD && CONFIG_CPU_CORTEX_M_HAS_SPLIM */

	/* reset CONTROL */
	__set_CONTROL(0U);

	/* reset PRIMASK/FAULTMASK */
	__enable_irq();
	__enable_fault_irq();

	/* set main stack pointer address (VT[0]) */
	__set_MSP(*(uintptr_t *)addr);

	/* jump to reset handler (VT[1]) */
	((void (*)(void))*(uintptr_t *)(addr + 4U))();

	CODE_UNREACHABLE;
}
