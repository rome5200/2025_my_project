// SPDX-License-Identifier: MIT
// Board: Zybo Z7-10
// Goal : Mirror PMOD inputs (Ch1) → LEDs (Ch2) using a single AXI GPIO (dual channel)
// Note : Structure intentionally matches the Zybo Z7-20 example style you shared
//        (BaseAddress + XGpio_CfgInitialize, channel-based direction, raw→LED write).

#include "xparameters.h"
#include "xgpio.h"
#include "sleep.h"
#include <stdint.h>

/* ---- AXI GPIO (듀얼채널) ----
 * Vivado BD에서 PMOD0(입력)를 AXI GPIO 채널1에, LED(출력)를 채널2에 연결했다고 가정.
 * xparameters.h에서 다음 심볼을 확인/수정하세요.
 */
#define GPIO_BASEADDR      XPAR_AXI_GPIO_0_BASEADDR      // 예: 0x41200000
#define GPIO_IS_DUAL       XPAR_AXI_GPIO_0_IS_DUAL          // 예: 1
#define GPIO_INTR_PRESENT  XPAR_AXI_GPIO_0_INTERRUPT_PRESENT

/* 채널 매핑 */
#define CH_PMOD  1   // 채널1: PMOD 입력 (최대 8bit 사용)
#define CH_LED   2   // 채널2: LED  출력 (Zybo Z7-10은 4개 LD0..LD3)

/* 폭(Width) 정의
 * IP에서 채널 폭을 32로 두는 경우가 흔하므로 안전한 마스크 유틸을 사용.
 */
#ifndef PMOD_WIDTH
  #define PMOD_WIDTH 4   // 입력은 4비트만 사용 (GND/VDD는 외부 처리)
#endif
#ifndef LED_WIDTH
  #define LED_WIDTH 4
#endif

/* 입력/출력 극성 및 매핑 설정 */
#define PMOD_ACTIVE_LOW  0   // 외부 보드가 Active-Low로 주면 1로 바꿔 뒤집기
#define LED_ACTIVE_HIGH  1   // Zybo Z7-10 LED는 일반적으로 Active-High

/* PMOD 하위 4비트를 LED로 보낼지 여부 (1이면 [PMOD[3:0]]→LED[3:0]) */

/* 안전한 마스크 계산 */
static inline uint32_t mask_n(uint32_t n) {
    if (n == 0) return 0u;
    if (n >= 32) return 0xFFFFFFFFu;
    return (1u << n) - 1u;
}

static inline uint32_t apply_polarity_in(uint32_t v) {
#if PMOD_ACTIVE_LOW
    return ~v;
#else
    return v;
#endif
}

static inline uint32_t apply_polarity_led(uint32_t v) {
#if LED_ACTIVE_HIGH
    return v;
#else
    return ~v;
#endif
}

static XGpio Gpio;

int main(void)
{
    /* BaseAddress 기반 초기화 (사용자 예시와 동일한 흐름) */
    XGpio_Config cfg;
    cfg.BaseAddress      = GPIO_BASEADDR;
    cfg.InterruptPresent = GPIO_INTR_PRESENT;
    cfg.IsDual           = GPIO_IS_DUAL;

    if (XGpio_CfgInitialize(&Gpio, &cfg, cfg.BaseAddress) != XST_SUCCESS)
        return XST_FAILURE;

    /* 방향 설정: PMOD=입력, LED=출력 */
    XGpio_SetDataDirection(&Gpio, CH_PMOD, 0xFFFFFFFFu);           // 전 비트 입력
    XGpio_SetDataDirection(&Gpio, CH_LED,  0x00000000u);           // 전 비트 출력

    /* 초기 LED OFF */
    uint32_t led_mask = mask_n(LED_WIDTH);
#if LED_ACTIVE_HIGH
    XGpio_DiscreteWrite(&Gpio, CH_LED, 0u);
#else
    XGpio_DiscreteWrite(&Gpio, CH_LED, led_mask);
#endif

    /* 메인 루프: PMOD 입력 → LED 출력 */
    while (1) {
        /* 1) PMOD 원시값 읽기 & 폭 제한 */
        uint32_t raw = XGpio_DiscreteRead(&Gpio, CH_PMOD);
        uint32_t pmod_masked = raw & mask_n(PMOD_WIDTH);

        /* 2) 입력 극성 보정 */
        uint32_t pmod_in = apply_polarity_in(pmod_masked) & mask_n(PMOD_WIDTH);

        /* 3) LED로 전달할 값 산출: PMOD[3:0] → LED[3:0] */
        uint32_t led_val   = pmod_in & mask_n(LED_WIDTH);

        /* 4) LED 극성 보정 후 출력 */
        uint32_t led_drive = apply_polarity_led(led_val) & mask_n(LED_WIDTH);
        XGpio_DiscreteWrite(&Gpio, CH_LED, led_drive);

        /* 폴링 주기 (전력/버스 사용량 고려) */
        usleep(500); // 0.5ms
    }

    return 0;
}
