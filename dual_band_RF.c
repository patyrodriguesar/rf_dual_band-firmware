/* ===================================================================================
   Federal University of Itajuba (UNIFEI)
   Postgraduate Program in Electrical Engineering - PPG-EEL

   Master's student: Patricia Rodrigues
   Supervisors UNIFEI: Prof. PhD. Otavio Gomes and Prof. PhD. Decio Renno
 
   rf_dual_band.c
	 
   Created on: January 15, 2026
	 
   Last modified: May 22, 2026

   Bare-Metal C Programming
	 
   MCU: STM32F103C8T6	 
   =================================================================================== */
	 
typedef unsigned char    uint8_t;
typedef unsigned short   uint16_t;
typedef unsigned int     uint32_t;
typedef unsigned long    uintptr_t;
typedef signed int       int32_t;

#define REG(addr)  (*(volatile uint32_t *)(uintptr_t)(addr))

#define FLASH_ACR   (0x40022000U + 0x00U)
#define RCC_CR      (0x40021000U + 0x00U)
#define RCC_CFGR    (0x40021000U + 0x04U)
#define RCC_APB2ENR (0x40021000U + 0x18U)
#define RCC_APB1ENR (0x40021000U + 0x1CU)
#define AFIO_MAPR   (0x40010000U + 0x04U)
#define GPIOA_CRL   (0x40010800U + 0x00U)
#define GPIOA_IDR   (0x40010800U + 0x08U)
#define GPIOA_BSRR  (0x40010800U + 0x10U)
#define GPIOB_CRL   (0x40010C00U + 0x00U)
#define GPIOB_IDR   (0x40010C00U + 0x08U)
#define GPIOB_BSRR  (0x40010C00U + 0x10U)
#define TIM3_CR1    (0x40000400U + 0x00U)
#define TIM3_DIER   (0x40000400U + 0x0CU)
#define TIM3_SR     (0x40000400U + 0x10U)
#define TIM3_EGR    (0x40000400U + 0x14U)
#define TIM3_PSC    (0x40000400U + 0x28U)
#define TIM3_ARR    (0x40000400U + 0x2CU)
#define I2C1_CR1    (0x40005400U + 0x00U)
#define I2C1_CR2    (0x40005400U + 0x04U)
#define I2C1_DR     (0x40005400U + 0x10U)
#define I2C1_SR1    (0x40005400U + 0x14U)
#define I2C1_SR2    (0x40005400U + 0x18U)
#define I2C1_CCR    (0x40005400U + 0x1CU)
#define I2C1_TRISE  (0x40005400U + 0x20U)
#define STK_CTRL    (0xE000E010U + 0x00U)
#define STK_LOAD    (0xE000E010U + 0x04U)
#define STK_VAL     (0xE000E010U + 0x08U)
#define NVIC_ISER0  (0xE000E100U + 0x00U)
#define IWDG_KR     (0x40003000U + 0x00U)
#define IWDG_PR     (0x40003000U + 0x04U)
#define IWDG_RLR    (0x40003000U + 0x08U)
#define IWDG_SR     (0x40003000U + 0x0CU)

#define FLASH_LATENCY_2WS       (2U << 0U)
#define FLASH_PRFTBE            (1U << 4U)
#define RCC_CR_HSEON            (1U << 16U)
#define RCC_CR_HSERDY           (1U << 17U)
#define RCC_CR_PLLON            (1U << 24U)
#define RCC_CR_PLLRDY           (1U << 25U)
#define RCC_CFGR_SW_MASK        (3U << 0U)
#define RCC_CFGR_SW_PLL         (2U << 0U)
#define RCC_CFGR_SWS_MASK       (3U << 2U)
#define RCC_CFGR_SWS_PLL        (2U << 2U)
#define RCC_CFGR_PPRE1_DIV2     (4U << 8U)
#define RCC_CFGR_PLLSRC_HSE     (1U << 16U)
#define RCC_CFGR_PLLMUL9        (7U << 18U)
#define RCC_CFGR_CONFIG_MASK    ((0xFU << 4U) | (0x7U << 8U) | (1U << 16U) | (1U << 17U) | (0xFU << 18U))
#define RCC_APB2_AFIOEN         (1U << 0U)
#define RCC_APB2_IOPAEN         (1U << 2U)
#define RCC_APB2_IOPBEN         (1U << 3U)
#define RCC_APB1_TIM3EN         (1U << 1U)
#define RCC_APB1_I2C1EN         (1U << 21U)
#define AFIO_SWJ_MASK           (7U << 24U)
#define AFIO_SWJ_NOJTAG         (2U << 24U)
#define GPIO_INPUT_PUPD         0x8U
#define GPIO_OUTPUT_PP_2MHZ     0x2U
#define GPIO_AF_OD_50MHZ        0xFU
#define GPIO_NIBBLE_MASK        0xFU
#define GPIO_SET(bsrr, pin)     (REG(bsrr) = (1U << (pin)))
#define GPIO_CLR(bsrr, pin)     (REG(bsrr) = (1U << ((pin) + 16U)))
#define I2C_CR1_PE              (1U << 0U)
#define I2C_CR1_START           (1U << 8U)
#define I2C_CR1_STOP            (1U << 9U)
#define I2C_CR1_SWRST           (1U << 15U)
#define I2C_SR1_SB              (1U << 0U)
#define I2C_SR1_ADDR            (1U << 1U)
#define I2C_SR1_BTF             (1U << 2U)
#define I2C_SR1_TXE             (1U << 7U)
#define I2C_SR1_BERR            (1U << 8U)
#define I2C_SR1_ARLO            (1U << 9U)
#define I2C_SR1_AF              (1U << 10U)
#define I2C_SR1_ERROR_MASK      (I2C_SR1_BERR | I2C_SR1_ARLO | I2C_SR1_AF)
#define I2C_SR2_BUSY            (1U << 1U)
#define STK_CTRL_ENABLE         (1U << 0U)
#define STK_CTRL_TICKINT        (1U << 1U)
#define STK_CTRL_CLKSRC         (1U << 2U)
#define TIM_CR1_CEN             (1U << 0U)
#define TIM_DIER_UIE            (1U << 0U)
#define TIM_SR_UIF              (1U << 0U)
#define TIM_EGR_UG              (1U << 0U)
#define IRQ_TIM3                29U
#define IWDG_KEY_UNLOCK         0x5555U
#define IWDG_KEY_KICK           0xAAAAU
#define IWDG_KEY_START          0xCCCCU
#define IWDG_SR_PVU             (1U << 0U)
#define IWDG_SR_RVU             (1U << 1U)
#define IWDG_KICK()             (REG(IWDG_KR) = IWDG_KEY_KICK)

#define STATUS_OK                0
#define STATUS_ERROR            -1
#define FAULT_CLOCK_HSE         (1U << 0U)
#define FAULT_CLOCK_PLL         (1U << 1U)
#define FAULT_CLOCK_SWITCH      (1U << 2U)
#define FAULT_I2C               (1U << 3U)
#define FAULT_OLED              (1U << 4U)
#define FAULT_RF_OVERRUN_1356   (1U << 5U)
#define FAULT_RF_OVERRUN_433    (1U << 6U)
#define FAULT_IWDG              (1U << 7U)
#define PIN_POWER               0U      
#define PIN_BUZZER              1U      
#define PIN_RF_1356             6U      
#define PIN_RF_433              0U      
#define PIN_I2C_SCL             6U    
#define PIN_I2C_SDA             7U      
#define SYSCLK_HZ               72000000U
#define PCLK1_HZ                36000000U
#define TIM3_SAMPLE_HZ          100000U
#define WINDOW_SAMPLES          1024U                 
#define CONFIRM_WINDOWS         3U                   
#define JAM_THRESHOLD_1356      ((WINDOW_SAMPLES * 80U) / 100U)
#define LOW_THRESHOLD_1356      ((WINDOW_SAMPLES * 20U) / 100U)
#define JAM_THRESHOLD_433       ((WINDOW_SAMPLES * 80U) / 100U)
#define LOW_THRESHOLD_433       ((WINDOW_SAMPLES * 20U) / 100U)
#define BAND_1356               0U
#define BAND_433                1U
#define BAND_COUNT              2U
#define I2C_SCL_HZ              100000U
#define I2C_TIMEOUT_MS          5U
#define OLED_ADDR               0x3CU
#define OLED_CTRL_CMD           0x00U
#define OLED_CTRL_DAT           0x40U
#define OLED_WIDTH              128U
#define OLED_CHAR_WIDTH         6U
#define BOOT_SCREEN_MS          1500U
#define DEBOUNCE_MS             30U
#define CLOCK_TIMEOUT_ITER      0x40000U
#define SWITCH_TIMEOUT_ITER     0x10000U
#define IWDG_UPDATE_TIMEOUT     0x10000U

typedef enum {
    RF_UNKNOWN = 0,
    RF_NORMAL  = 1,
    RF_JAMMING = 2,
    RF_FAULT   = 3
} rf_state_t;

typedef struct {
    uint16_t jam_threshold;
    uint16_t low_threshold;
    uint8_t  confirm_windows;
} rf_config_t;

typedef struct {
    volatile uint16_t sample_count;
    volatile uint16_t high_count;
    volatile uint16_t ready_high_count;
    volatile uint8_t  ready;
    volatile uint8_t  overrun;

    rf_state_t state;
    rf_state_t candidate;
    uint8_t    candidate_count;
    uint16_t   last_high_count;   
} rf_detector_t;

typedef struct {
    uint32_t last_raw;
    uint32_t stable;
    uint32_t last_change_ms;
} button_t;


static volatile uint32_t  g_ms_tick          = 0U;
static volatile uint32_t  g_system_faults    = 0U;
static volatile uint32_t  g_powered          = 1U;
static volatile uint32_t  g_sampling_enabled = 0U;
static          rf_detector_t g_rf[BAND_COUNT];
static          button_t  g_button;
static          uint32_t  g_oled_ok          = 0U;

static const rf_config_t g_rf_cfg[BAND_COUNT] = {
    { JAM_THRESHOLD_1356, LOW_THRESHOLD_1356, CONFIRM_WINDOWS },
    { JAM_THRESHOLD_433,  LOW_THRESHOLD_433,  CONFIRM_WINDOWS }
};

static const uint32_t g_band_overrun_fault[BAND_COUNT] = {
    FAULT_RF_OVERRUN_1356,
    FAULT_RF_OVERRUN_433
};

void SysTick_Handler(void);
void TIM3_IRQHandler(void);

static inline void irq_disable(void) { __asm volatile ("cpsid i" ::: "memory"); }
static inline void irq_enable(void)  { __asm volatile ("cpsie i" ::: "memory"); }
static inline void cpu_wfi(void)     { __asm volatile ("wfi"     ::: "memory"); }

static int32_t wait_bit_set(uint32_t reg, uint32_t mask, uint32_t timeout) {
    while ((REG(reg) & mask) == 0U) {
        if (timeout == 0U) { return STATUS_ERROR; }
        timeout--;
    }
    return STATUS_OK;
}

static int32_t iwdg_init(void) {
    uint32_t t = IWDG_UPDATE_TIMEOUT;
    int32_t status = STATUS_OK;

    REG(IWDG_KR)  = IWDG_KEY_UNLOCK;
    REG(IWDG_PR)  = 3U;      
    REG(IWDG_RLR) = 1250U;   

    while ((REG(IWDG_SR) & (IWDG_SR_PVU | IWDG_SR_RVU)) != 0U) {
        if (t == 0U) {
            g_system_faults |= FAULT_IWDG;
            status = STATUS_ERROR;
            break;
        }
        t--;
    }

    REG(IWDG_KR) = IWDG_KEY_START;
    return status;
}

static int32_t clock_init(void) {
    REG(FLASH_ACR) = FLASH_LATENCY_2WS | FLASH_PRFTBE;

    REG(RCC_CR) |= RCC_CR_HSEON;
    if (wait_bit_set(RCC_CR, RCC_CR_HSERDY, CLOCK_TIMEOUT_ITER) != STATUS_OK) {
        g_system_faults |= FAULT_CLOCK_HSE;
        return STATUS_ERROR;
    }

    {
        uint32_t cfgr = REG(RCC_CFGR);
        cfgr &= ~RCC_CFGR_CONFIG_MASK;
        cfgr |=  RCC_CFGR_PLLSRC_HSE | RCC_CFGR_PLLMUL9 | RCC_CFGR_PPRE1_DIV2;
        REG(RCC_CFGR) = cfgr;
    }

    REG(RCC_CR) |= RCC_CR_PLLON;
    if (wait_bit_set(RCC_CR, RCC_CR_PLLRDY, CLOCK_TIMEOUT_ITER) != STATUS_OK) {
        g_system_faults |= FAULT_CLOCK_PLL;
        return STATUS_ERROR;
    }

    REG(RCC_CFGR) = (REG(RCC_CFGR) & ~RCC_CFGR_SW_MASK) | RCC_CFGR_SW_PLL;

    {
        uint32_t t = SWITCH_TIMEOUT_ITER;
        while ((REG(RCC_CFGR) & RCC_CFGR_SWS_MASK) != RCC_CFGR_SWS_PLL) {
            if (t == 0U) {
                g_system_faults |= FAULT_CLOCK_SWITCH;
                return STATUS_ERROR;
            }
            t--;
        }
    }
    return STATUS_OK;
}

static void systick_init(void) {
    REG(STK_LOAD) = (SYSCLK_HZ / 1000U) - 1U;
    REG(STK_VAL)  = 0U;
    REG(STK_CTRL) = STK_CTRL_CLKSRC | STK_CTRL_TICKINT | STK_CTRL_ENABLE;
}

void SysTick_Handler(void) { g_ms_tick++; }

static inline uint32_t millis(void)         { return g_ms_tick; }
static inline uint32_t elapsed(uint32_t t0) { return g_ms_tick - t0; }

static void delay_ms(uint32_t ms) {
    const uint32_t t0 = g_ms_tick;
    while ((g_ms_tick - t0) < ms) { IWDG_KICK(); }
}

static void gpio_init(void) {
    REG(RCC_APB2ENR) |= RCC_APB2_AFIOEN | RCC_APB2_IOPAEN | RCC_APB2_IOPBEN;

    REG(AFIO_MAPR) = (REG(AFIO_MAPR) & ~AFIO_SWJ_MASK) | AFIO_SWJ_NOJTAG;

    REG(GPIOA_CRL) = (REG(GPIOA_CRL) & ~(GPIO_NIBBLE_MASK << (PIN_POWER * 4U)))
                   | (GPIO_INPUT_PUPD << (PIN_POWER * 4U));
    GPIO_SET(GPIOA_BSRR, PIN_POWER);

    REG(GPIOA_CRL) = (REG(GPIOA_CRL) & ~(GPIO_NIBBLE_MASK << (PIN_BUZZER * 4U)))
                   | (GPIO_OUTPUT_PP_2MHZ << (PIN_BUZZER * 4U));
    GPIO_CLR(GPIOA_BSRR, PIN_BUZZER);

    REG(GPIOA_CRL) = (REG(GPIOA_CRL) & ~(GPIO_NIBBLE_MASK << (PIN_RF_1356 * 4U)))
                   | (GPIO_INPUT_PUPD << (PIN_RF_1356 * 4U));
    GPIO_CLR(GPIOA_BSRR, PIN_RF_1356);

    REG(GPIOB_CRL) = (REG(GPIOB_CRL) & ~(GPIO_NIBBLE_MASK << (PIN_RF_433 * 4U)))
                   | (GPIO_INPUT_PUPD << (PIN_RF_433 * 4U));
    GPIO_CLR(GPIOB_BSRR, PIN_RF_433);

    REG(GPIOB_CRL) = (REG(GPIOB_CRL) & ~((GPIO_NIBBLE_MASK << (PIN_I2C_SCL * 4U)) |
                                          (GPIO_NIBBLE_MASK << (PIN_I2C_SDA * 4U))))
                   | ((GPIO_AF_OD_50MHZ << (PIN_I2C_SCL * 4U)) |
                      (GPIO_AF_OD_50MHZ << (PIN_I2C_SDA * 4U)));
}

static void tim3_init(void) {
    REG(RCC_APB1ENR) |= RCC_APB1_TIM3EN;
    REG(TIM3_CR1)     = 0U;
    REG(TIM3_PSC)     = 0U;
    REG(TIM3_ARR)     = (SYSCLK_HZ / TIM3_SAMPLE_HZ) - 1U;
    REG(TIM3_EGR)     = TIM_EGR_UG;
    REG(TIM3_SR)      = 0U;
    REG(TIM3_DIER)    = TIM_DIER_UIE;
    REG(NVIC_ISER0)   = (1U << IRQ_TIM3);
    REG(TIM3_CR1)     = TIM_CR1_CEN;
}

static inline void rf_push_sample(rf_detector_t *d, uint32_t bit) {
    if (bit != 0U) { d->high_count++; }
    d->sample_count++;

    if (d->sample_count >= WINDOW_SAMPLES) {
        if (d->ready != 0U) { d->overrun = 1U; }
        d->ready_high_count = d->high_count;
        d->high_count       = 0U;
        d->sample_count     = 0U;
        d->ready            = 1U;
    }
}

void TIM3_IRQHandler(void) {
    if ((REG(TIM3_SR) & TIM_SR_UIF) == 0U) { return; }
    REG(TIM3_SR) = ~TIM_SR_UIF;

    if (g_sampling_enabled == 0U) { return; }

    {
        const uint32_t idr_a = REG(GPIOA_IDR);
        const uint32_t idr_b = REG(GPIOB_IDR);
        rf_push_sample(&g_rf[BAND_1356], (idr_a >> PIN_RF_1356) & 1U);
        rf_push_sample(&g_rf[BAND_433 ], (idr_b >> PIN_RF_433 ) & 1U);
    }
}

static void rf_reset(rf_detector_t *d) {
    irq_disable();
    d->sample_count     = 0U;
    d->high_count       = 0U;
    d->ready_high_count = 0U;
    d->ready            = 0U;
    d->overrun          = 0U;
    d->state            = RF_UNKNOWN;
    d->candidate        = RF_UNKNOWN;
    d->candidate_count  = 0U;
    d->last_high_count  = 0U;
    irq_enable();
}

static rf_state_t classify(uint16_t high, const rf_config_t *cfg) {
    if (high >= cfg->jam_threshold) { return RF_JAMMING;   }
    if (high <= cfg->low_threshold) { return RF_FAULT; }
    return RF_NORMAL;
}

static uint32_t rf_process(rf_detector_t *d, const rf_config_t *cfg, uint32_t overrun_fault_bit) {
    uint16_t   high;
    uint8_t    had_overrun;
    rf_state_t observed;

    if (d->ready == 0U) { return 0U; }

    irq_disable();
    high        = d->ready_high_count;
    d->ready    = 0U;
    had_overrun = d->overrun;
    d->overrun  = 0U;
    irq_enable();

    if (had_overrun != 0U) { g_system_faults |= overrun_fault_bit; }

    observed = classify(high, cfg);
    d->last_high_count = high;

    if (observed == d->candidate) {
        if (d->candidate_count < cfg->confirm_windows) {
            d->candidate_count++;
        }
    } else {
        d->candidate       = observed;
        d->candidate_count = 1U;
    }

    if ((d->candidate_count >= cfg->confirm_windows) &&
        (d->state != d->candidate)) {
        d->state = d->candidate;
        return 1U;
    }
    return 0U;
}

static void i2c_init(void) {
    REG(RCC_APB1ENR) |= RCC_APB1_I2C1EN;

    REG(I2C1_CR1) |= I2C_CR1_SWRST;
    REG(I2C1_CR1) &= ~I2C_CR1_SWRST;
    REG(I2C1_CR1) &= ~I2C_CR1_PE;

    REG(I2C1_CR2)   = PCLK1_HZ / 1000000U;
    REG(I2C1_CCR)   = PCLK1_HZ / (2U * I2C_SCL_HZ);
    REG(I2C1_TRISE) = (PCLK1_HZ / 1000000U) + 1U;

    REG(I2C1_CR1) |= I2C_CR1_PE;
}

static int32_t i2c_wait_flag(uint32_t flag) {
    const uint32_t t0 = millis();
    for (;;) {
        const uint32_t sr1 = REG(I2C1_SR1);
        if ((sr1 & I2C_SR1_ERROR_MASK) != 0U) {
            REG(I2C1_SR1) &= ~I2C_SR1_ERROR_MASK;
            g_system_faults |= FAULT_I2C;
            return STATUS_ERROR;
        }
        if ((sr1 & flag) != 0U) { return STATUS_OK; }
        if (elapsed(t0) > I2C_TIMEOUT_MS) {
            g_system_faults |= FAULT_I2C;
            return STATUS_ERROR;
        }
    }
}

static int32_t i2c_wait_busy_clear(void) {
    const uint32_t t0 = millis();
    while ((REG(I2C1_SR2) & I2C_SR2_BUSY) != 0U) {
        if (elapsed(t0) > I2C_TIMEOUT_MS) {
            g_system_faults |= FAULT_I2C;
            return STATUS_ERROR;
        }
    }
    return STATUS_OK;
}

static int32_t i2c_write(uint8_t addr, uint8_t ctrl, const uint8_t *data, uint32_t n, uint32_t data_repeated) {
    int32_t  rc;
    uint32_t started = 0U;

    if ((n > 0U) && (data == 0)) { return STATUS_ERROR; }

    rc = i2c_wait_busy_clear();
    if (rc != STATUS_OK) { return rc; }

    REG(I2C1_CR1) |= I2C_CR1_START;
    started = 1U;

    rc = i2c_wait_flag(I2C_SR1_SB);
    if (rc != STATUS_OK) { goto done; }

    REG(I2C1_DR) = (uint8_t)(addr << 1U);
    rc = i2c_wait_flag(I2C_SR1_ADDR);
    if (rc != STATUS_OK) { goto done; }
    (void)REG(I2C1_SR1);
    (void)REG(I2C1_SR2);

    REG(I2C1_DR) = ctrl;

    {
        uint32_t i;
        for (i = 0U; i < n; i++) {
            rc = i2c_wait_flag(I2C_SR1_TXE);
            if (rc != STATUS_OK) { goto done; }
            REG(I2C1_DR) = data_repeated ? data[0] : data[i];
        }
    }

    rc = i2c_wait_flag(I2C_SR1_BTF);

done:
    if (started != 0U) { REG(I2C1_CR1) |= I2C_CR1_STOP; }
    return rc;
}

static const uint8_t SSD1306_INIT[] = {
    0xAEU, 0xD5U, 0x80U, 0xA8U, 0x3FU, 0xD3U, 0x00U,
    0x40U, 0x8DU, 0x14U, 0x20U, 0x02U, 0xA1U, 0xC8U,
    0xDAU, 0x12U, 0x81U, 0xCFU, 0xD9U, 0xF1U, 0xDBU,
    0x40U, 0xA4U, 0xA6U, 0xAFU
};

static int32_t oled_write_checked(uint8_t ctrl, const uint8_t *data, uint32_t n, uint32_t repeated) {
    int32_t rc;
    if (g_oled_ok == 0U) { return STATUS_ERROR; }

    rc = i2c_write(OLED_ADDR, ctrl, data, n, repeated);
    if (rc != STATUS_OK) {
        g_oled_ok = 0U;
        g_system_faults |= FAULT_OLED;
    }
    return rc;
}

static int32_t oled_cmd_list(const uint8_t *cmds, uint32_t n) {
    return oled_write_checked(OLED_CTRL_CMD, cmds, n, 0U);
}

static int32_t oled_data(const uint8_t *data, uint32_t n) {
    return oled_write_checked(OLED_CTRL_DAT, data, n, 0U);
}

static int32_t oled_data_fill(uint8_t v, uint32_t n) {
    return oled_write_checked(OLED_CTRL_DAT, &v, n, 1U);
}

static void oled_set_cursor(uint32_t page, uint32_t col) {
    uint8_t cmds[3];
    if ((page > 7U) || (col >= OLED_WIDTH)) { return; }
    cmds[0] = (uint8_t)(0xB0U | (page & 0x07U));
    cmds[1] = (uint8_t)(col & 0x0FU);
    cmds[2] = (uint8_t)(0x10U | (col >> 4U));
    (void)oled_cmd_list(cmds, 3U);
}

static void oled_clear(void) {
    uint32_t p;
    for (p = 0U; p < 8U; p++) {
        oled_set_cursor(p, 0U);
        (void)oled_data_fill(0x00U, OLED_WIDTH);
    }
}

static int32_t oled_init(void) {
    delay_ms(100U);
    g_oled_ok = 1U;
    if (oled_cmd_list(SSD1306_INIT, sizeof(SSD1306_INIT)) != STATUS_OK) {
        g_oled_ok = 0U;
        g_system_faults |= FAULT_OLED;
        return STATUS_ERROR;
    }
    oled_clear();
    return STATUS_OK;
}

#define FONT_FIRST  32U
#define FONT_SIZE   91U

static const uint8_t g_font[FONT_SIZE][5] = {
    {0x00,0x00,0x00,0x00,0x00},{0x00,0x00,0x5F,0x00,0x00},
    {0x00,0x07,0x00,0x07,0x00},{0x14,0x7F,0x14,0x7F,0x14},
    {0x24,0x2A,0x7F,0x2A,0x12},{0x23,0x13,0x08,0x64,0x62},
    {0x36,0x49,0x55,0x22,0x50},{0x00,0x05,0x03,0x00,0x00},
    {0x00,0x1C,0x22,0x41,0x00},{0x00,0x41,0x22,0x1C,0x00},
    {0x14,0x08,0x3E,0x08,0x14},{0x08,0x08,0x3E,0x08,0x08},
    {0x00,0x50,0x30,0x00,0x00},{0x08,0x08,0x08,0x08,0x08},
    {0x00,0x60,0x60,0x00,0x00},{0x20,0x10,0x08,0x04,0x02},
    {0x3E,0x51,0x49,0x45,0x3E},{0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46},{0x21,0x41,0x45,0x4B,0x31},
    {0x18,0x14,0x12,0x7F,0x10},{0x27,0x45,0x45,0x45,0x39},
    {0x3C,0x4A,0x49,0x49,0x30},{0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36},{0x06,0x49,0x49,0x29,0x1E},
    {0x00,0x36,0x36,0x00,0x00},{0x00,0x56,0x36,0x00,0x00},
    {0x08,0x14,0x22,0x41,0x00},{0x14,0x14,0x14,0x14,0x14},
    {0x00,0x41,0x22,0x14,0x08},{0x02,0x01,0x51,0x09,0x06},
    {0x32,0x49,0x79,0x41,0x3E},{0x7E,0x11,0x11,0x11,0x7E},
    {0x7F,0x49,0x49,0x49,0x36},{0x3E,0x41,0x41,0x41,0x22},
    {0x7F,0x41,0x41,0x22,0x1C},{0x7F,0x49,0x49,0x49,0x41},
    {0x7F,0x09,0x09,0x09,0x01},{0x3E,0x41,0x49,0x49,0x7A},
    {0x7F,0x08,0x08,0x08,0x7F},{0x00,0x41,0x7F,0x41,0x00},
    {0x20,0x40,0x41,0x3F,0x01},{0x7F,0x08,0x14,0x22,0x41},
    {0x7F,0x40,0x40,0x40,0x40},{0x7F,0x02,0x0C,0x02,0x7F},
    {0x7F,0x04,0x08,0x10,0x7F},{0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,0x09,0x09,0x09,0x06},{0x3E,0x41,0x51,0x21,0x5E},
    {0x7F,0x09,0x19,0x29,0x46},{0x46,0x49,0x49,0x49,0x31},
    {0x01,0x01,0x7F,0x01,0x01},{0x3F,0x40,0x40,0x40,0x3F},
    {0x1F,0x20,0x40,0x20,0x1F},{0x3F,0x40,0x38,0x40,0x3F},
    {0x63,0x14,0x08,0x14,0x63},{0x07,0x08,0x70,0x08,0x07},
    {0x61,0x51,0x49,0x45,0x43},{0x00,0x7F,0x41,0x00,0x00},
    {0x02,0x04,0x08,0x10,0x20},{0x00,0x41,0x7F,0x00,0x00},
    {0x04,0x02,0x01,0x02,0x04},{0x40,0x40,0x40,0x40,0x40},
    {0x00,0x01,0x02,0x04,0x00},{0x20,0x54,0x54,0x54,0x78},
    {0x7F,0x48,0x44,0x44,0x38},{0x38,0x44,0x44,0x44,0x20},
    {0x38,0x44,0x44,0x48,0x7F},{0x38,0x54,0x54,0x54,0x18},
    {0x08,0x7E,0x09,0x01,0x02},{0x0C,0x52,0x52,0x52,0x3E},
    {0x7F,0x08,0x04,0x04,0x78},{0x00,0x44,0x7D,0x40,0x00},
    {0x20,0x40,0x44,0x3D,0x00},{0x7F,0x10,0x28,0x44,0x00},
    {0x00,0x41,0x7F,0x40,0x00},{0x7C,0x04,0x18,0x04,0x78},
    {0x7C,0x08,0x04,0x04,0x78},{0x38,0x44,0x44,0x44,0x38},
    {0x7C,0x14,0x14,0x14,0x08},{0x08,0x14,0x14,0x18,0x7C},
    {0x7C,0x08,0x04,0x04,0x08},{0x48,0x54,0x54,0x54,0x20},
    {0x04,0x3F,0x44,0x40,0x20},{0x3C,0x40,0x40,0x20,0x7C},
    {0x1C,0x20,0x40,0x20,0x1C},{0x3C,0x40,0x30,0x40,0x3C},
    {0x44,0x28,0x10,0x28,0x44},{0x0C,0x50,0x50,0x50,0x3C},
    {0x44,0x64,0x54,0x4C,0x44}
};

static void oled_print(uint32_t page, uint32_t col, const char *s) {
    uint8_t buf[OLED_CHAR_WIDTH];

    if ((page > 7U) || (col >= OLED_WIDTH) || (s == 0)) { return; }
    oled_set_cursor(page, col);

    while ((*s != '\0') && ((col + OLED_CHAR_WIDTH) <= OLED_WIDTH)) {
        const int32_t  sidx = (int32_t)(uint8_t)*s - (int32_t)FONT_FIRST;
        const uint32_t idx  = ((sidx < 0) || (sidx >= (int32_t)FONT_SIZE)) ? 0U : (uint32_t)sidx;
        uint32_t i;
        for (i = 0U; i < 5U; i++) { buf[i] = g_font[idx][i]; }
        buf[5] = 0x00U;
        (void)oled_data(buf, OLED_CHAR_WIDTH);
        s++;
        col += OLED_CHAR_WIDTH;
    }
}

static void oled_logo_unifei(uint32_t page, uint32_t col) {
    static const uint8_t p0[15] = {
        0x30, 0xF8, 0x18, 0x0E, 0x07,
        0xF3, 0x92, 0xD2, 0xB2, 0x9B,
        0x07, 0x0E, 0x18, 0xF8, 0x30
    };
    static const uint8_t p1[15] = {
        0x06, 0x0F, 0x0C, 0x38, 0x70,
        0x67, 0x2C, 0x26, 0x25, 0x64,
        0x70, 0x38, 0x0C, 0x0F, 0x06
    };
    if ((page > 6U) || (col > (OLED_WIDTH - 15U))) {
        return;
    }
    oled_set_cursor(page, col);
    (void)oled_data(p0, 15U);
    oled_set_cursor(page + 1U, col);
    (void)oled_data(p1, 15U);
}

static const char *state_text(rf_state_t s) {
    const char *t;
    switch (s) {
    case RF_JAMMING:   t = "JAMMING!  "; break;
    case RF_FAULT:     t = "FAULT     "; break;
    case RF_NORMAL:    t = "NORMAL    "; break;
    case RF_UNKNOWN:
    default:           t = "Waiting..."; break;
    }
    return t;
}

static void display_header(void) {
    oled_clear();
	oled_logo_unifei(0U, 94U);       
	oled_print(3U, 0U, "13.56 MHz:");
    oled_print(5U, 0U, "  433 MHz:");
	oled_print(7U, 0U, "UNIFEI and Clavis");
}

static void display_states(void) {
    oled_print(3U, 65U, state_text(g_rf[BAND_1356].state));
    oled_print(5U, 65U, state_text(g_rf[BAND_433 ].state));
}

static void display_high_counts(void) {
    char     buf[6];
    uint32_t b;
    for (b = 0U; b < BAND_COUNT; b++) {
        uint32_t v   = g_rf[b].last_high_count;
        uint32_t pos = 5U;
        buf[5] = '\0';
        if (v == 0U) {
            buf[4] = '0'; pos = 4U;
        } else {
            while ((v > 0U) && (pos > 0U)) {
                pos--;
                buf[pos] = (char)('0' + (v % 10U));
                v /= 10U;
            }
        }       
    }
}

static void buzzer_update(void) {
    if ((g_powered != 0U) && ((g_rf[BAND_1356].state == RF_JAMMING) || (g_rf[BAND_433 ].state == RF_JAMMING))) {
        GPIO_SET(GPIOA_BSRR, PIN_BUZZER);
    } else {
        GPIO_CLR(GPIOA_BSRR, PIN_BUZZER);
    }
}

static uint32_t button_pressed_edge(void) {
    const uint32_t now = millis();
    const uint32_t raw = ((REG(GPIOA_IDR) & (1U << PIN_POWER)) == 0U) ? 1U : 0U;

    if (raw != g_button.last_raw) {
        g_button.last_raw       = raw;
        g_button.last_change_ms = now;
        return 0U;
    }
    if (((now - g_button.last_change_ms) >= DEBOUNCE_MS) && (raw != g_button.stable)) {
        g_button.stable = raw;
        if (raw == 1U) { return 1U; }
    }
    return 0U;
}

static void standby_exit(void) {
    irq_disable();
    g_powered          = 1U;
    g_sampling_enabled = 0U;
    irq_enable();
    rf_reset(&g_rf[BAND_1356]);
    rf_reset(&g_rf[BAND_433 ]);
    GPIO_CLR(GPIOA_BSRR, PIN_BUZZER);
    display_header();
    display_states();
    irq_disable();
    g_sampling_enabled = 1U;
    irq_enable();
}

static void standby_enter(void) {
    irq_disable();
    g_sampling_enabled = 0U;
    g_powered          = 0U;
    irq_enable();

    GPIO_CLR(GPIOA_BSRR, PIN_BUZZER);
    oled_clear();
    oled_logo_unifei(0U, 94U);       
    oled_print(3U, 0U, "Standby");
    oled_print(5U, 0U, "Press to turn ON");
}

int main(void) {
    (void)iwdg_init();  

    if (clock_init() != STATUS_OK) {
        for (;;) { }
    }

    systick_init();
    gpio_init();
    i2c_init();
    (void)oled_init();
    tim3_init();

    if (g_oled_ok != 0U) {
		oled_logo_unifei(0U, 94U);
        oled_print(3U, 12U, "UNIFEI and Clavis");
        oled_print(5U, 12U, "Dual-Band RF");
        delay_ms(BOOT_SCREEN_MS);
    }
		
	  standby_exit();   

    for (;;) {
        uint32_t changed_1356 = 0U;
        uint32_t changed_433  = 0U;
        uint32_t pressed;
        uint32_t no_work;

        IWDG_KICK();

        if (g_powered != 0U) {
            changed_1356 = rf_process(&g_rf[BAND_1356], &g_rf_cfg[BAND_1356], g_band_overrun_fault[BAND_1356]);
            changed_433  = rf_process(&g_rf[BAND_433 ], &g_rf_cfg[BAND_433 ], g_band_overrun_fault[BAND_433 ]);
        }

        pressed = button_pressed_edge();
        if (pressed != 0U) {
            if (g_powered != 0U) { standby_enter(); }
            else                 { standby_exit();  }
        }

        if ((g_powered != 0U) && ((changed_1356 != 0U) || (changed_433 != 0U))) {
            display_states();
            display_high_counts();
            buzzer_update();
        }

        irq_disable();
        no_work = ((g_rf[BAND_1356].ready == 0U) && (g_rf[BAND_433 ].ready == 0U)) ? 1U : 0U;
        if (no_work != 0U) { cpu_wfi(); }
        irq_enable();
    }
}
