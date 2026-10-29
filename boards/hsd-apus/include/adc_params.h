/*
 * SPDX-FileCopyrightText: 2026 Jakob Jungreuthmayer
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_hsd-apus
 * @{
 *
 * @file
 * @brief       Configuration of SAUL mapped ADC channels
 *
 * @author      Jakob Jungreuthmayer
 */

#include "board.h"
#include "saul/periph.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    ADC configuration
 */
static const saul_adc_params_t saul_adc_params[] =
{
    {
        .name = "POTI",
        .line = ADC_LINE(0),
        .res  = ADC_RES_12BIT,
    },
};

#ifdef __cplusplus
}
#endif

/** @} */
