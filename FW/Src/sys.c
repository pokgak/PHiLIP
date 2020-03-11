/*
 * Copyright 2020 Kevin Weiss for HAW Hamburg
 *
 * This file is subject to the terms and conditions of the MIT License. See the
 * file LICENSE in the top level directory for more details.
 * SPDX-License-Identifier:    MIT
 */

/**
 ******************************************************************************
 * @addtogroup periph
 * @{
 * @file			sys.c
 * @author			Kevin Weiss
 * @date			13.02.2019
 * @brief			System control and management.
 * @details			Deals with build times, serial numbers, ticks, etc.
 * @}
 ******************************************************************************
 */

/* Includes *******************************************************************/
#include <errno.h>
#include <stdint.h>
#include <string.h>

#include "stm32f1xx_hal.h"
#include "build_defs.h"

#include "PHiLIP_typedef.h"
#include "PHiLIP_map.h"
#include "app_common.h"
#include "app_defaults.h"
#include "port.h"

#include "sys.h"

/* Private variables **********************************************************/
static sys_t *sys_reg;

/******************************************************************************/
/*           Initialization                                                   */
/******************************************************************************/
void init_sys(map_t *reg) {
	sys_reg = &(reg->sys);

	uint32_t tmp[3];
	tmp[0] = HAL_GetUIDw0();
	tmp[1] = HAL_GetUIDw1();
	tmp[2] = HAL_GetUIDw2();
	memcpy(sys_reg->sn, tmp, sizeof(tmp));

	sys_reg->fw_rev[1] = FW_REV_PATCH;
	sys_reg->fw_rev[2] = FW_REV_MINOR;
	sys_reg->fw_rev[3] = FW_REV_MAJOR;
    sys_reg->fw_rev[0] = 0;

	sys_reg->if_rev[1] = IF_VERSION_PATCH;
	sys_reg->if_rev[2] = IF_VERSION_MINOR;
	sys_reg->if_rev[3] = IF_VERSION_MAJOR;
    sys_reg->if_rev[0] = 0;

	sys_reg->build_time.second = BUILD_SEC;
	sys_reg->build_time.minute = BUILD_MIN;
	sys_reg->build_time.hour = BUILD_HOUR;
	sys_reg->build_time.day_of_month = BUILD_DAY;
	sys_reg->build_time.month = BUILD_MONTH;
	sys_reg->build_time.year = BUILD_YEAR;

	sys_reg->status.board = BOARD_ID;

	sys_reg->sys_clk = SystemCoreClock;

	commit_sys();
}

error_t commit_sys() {
	sys_t *sys_inst = sys_reg;

	if (!sys_inst->mode.init) {
		if (sys_inst->mode.dut_rst) {
			HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_RESET);
		} else {
			HAL_GPIO_WritePin(DUT_RST_GPIO_Port, DUT_RST_Pin, GPIO_PIN_SET);
		}
		sys_inst->mode.init = 1;
	}
	sys_inst->status.update = 0;

	return 0;
}

/******************************************************************************/
/*           Functions                                                        */
/******************************************************************************/
void update_tick() {
	 sys_reg->tick = get_tick();
}
