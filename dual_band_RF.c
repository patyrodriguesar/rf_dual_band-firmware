/* ===================================================================================
   Federal University of Itajuba (UNIFEI)
   Postgraduate Program in Electrical Engineering - PPG-EEL
 
   Master’s student: Patrícia Rodrigues
   Supervisors: Prof. PhD. Otávio Gomes and Prof. PhD. Décio Rennó
   =================================================================================== */

/* -----------------------------------------------------------------------------------
   dual_band_RF.c:
	 
	 Bare-Metal C Programming for Dual-Band RF Detector [13.56 MHz and 433 MHz]
   
	 Created on: April 9, 2026
	 
   Last modified: April 22, 2026

   ARM microcontroller: STM32F103C8T6	
   ----------------------------------------------------------------------------------- */

#define REG(addr) (*((volatile unsigned int*)(addr)))

/* -----------------------------------------------------------------------------------
   ARM Cortex-M3: char = 8bits, short = 16bits, int = 32bits, long = 32bits   
   ----------------------------------------------------------------------------------- */
typedef unsigned char      uint8_t;
typedef unsigned int       uint32_t;
typedef signed int         int32_t;

/* -----------------------------------------------------------------------------------
   Register addresses - RM0008 STM32F10x Reference Manual    
   ----------------------------------------------------------------------------------- */
#define RCC_APB2ENR  (0x40021000 + 0x18)  
#define RCC_APB1ENR  (0x40021000 + 0x1C)                                            
#define AFIO_MAPR    (0x40010000 + 0x04)                                            
#define GPIOA_CRL    (0x40010800 + 0x00)  
#define GPIOA_IDR    (0x40010800 + 0x08)   
#define GPIOA_ODR    (0x40010800 + 0x0C)                                             
#define GPIOB_CRL    (0x40010C00 + 0x00)   
#define GPIOB_IDR    (0x40010C00 + 0x08)   																					 
#define I2C_CR1      (0x40005400 + 0x00)   
#define I2C_CR2      (0x40005400 + 0x04)  
#define I2C_DR       (0x40005400 + 0x10)   
#define I2C_SR1      (0x40005400 + 0x14) 
#define I2C_SR2      (0x40005400 + 0x18)   
#define I2C_CCR      (0x40005400 + 0x1C)   
#define I2C_TRISE    (0x40005400 + 0x20)                                             
#define STK_CTRL     (0xE000E010 + 0x00)   
#define STK_LOAD     (0xE000E010 + 0x04)   
#define STK_VAL      (0xE000E010 + 0x08)   

/* -----------------------------------------------------------------------------------
   Bit masks     
   ----------------------------------------------------------------------------------- */
#define AFIOEN       (1U << 0)          
#define IOPAEN       (1U << 2)      
#define IOPBEN       (1U << 3)      
#define I2C1EN       (1U << 21)                          
#define JTAG_GPIO    (0x02000000U)
#define CTRL_ENABLE  (1U << 0)         
#define CTRL_TICKINT (1U << 1)     
#define CTRL_CLKSRC  (1U << 2)     
#define CR1_PE       (1U << 0)      
#define CR1_START    (1U << 8)       
#define CR1_STOP     (1U << 9)        
#define SR1_SB       (1U << 0)     
#define SR1_ADDR     (1U << 1)            
#define SR1_BTF      (1U << 2)                                        
#define SR2_BUSY     (1U << 1)      

/* -----------------------------------------------------------------------------------
   Constants
   ----------------------------------------------------------------------------------- */
#define OLED_ADDR    0x3C                 
#define OLED_CMD     0x00                  
#define OLED_DAT     0x40                  
#define POWER        0                     
#define BUZZER       1                   
#define RF_IN        0                   
#define NORMAL       0                   
#define JAMMING      1                    
#define FAULT        2                    
#define WINDOW_BYTES 16                    
#define WINDOW_BITS (WINDOW_BYTES * 8)   

/* -----------------------------------------------------------------------------------
   Global variables
   ----------------------------------------------------------------------------------- */
volatile uint32_t  ms_tick       = 0;
uint8_t            window[WINDOW_BYTES];
uint32_t           window_idx    = 0;
uint8_t            current_byte  = 0;
uint32_t           bit_pos       = 0;
int32_t            rf_state      = -1;
uint32_t           powered       = 1;

/* -----------------------------------------------------------------------------------
   SysTick 
   ----------------------------------------------------------------------------------- */
void SysTick_Handler(void) {
    ms_tick++;                             
}

void delay(uint32_t ms) {
    uint32_t start = ms_tick;
    while ((ms_tick - start) < ms);
}

/* -----------------------------------------------------------------------------------
   Hardware initialization

   Pin mapping:
   PA0 = POWER button (input pull-up)
   PA1 = Buzzer       (output push-pull 2MHz)
   PB0 = RF signal    (input floating)
   PB6 = SCL I2C1     (AF open-drain 50MHz)
   PB7 = SDA I2C1     (AF open-drain 50MHz)
   ----------------------------------------------------------------------------------- */
void hardware_init(void) {
    REG(RCC_APB2ENR) |= AFIOEN | IOPAEN | IOPBEN;
    REG(RCC_APB1ENR) |= I2C1EN;
    REG(AFIO_MAPR) |= JTAG_GPIO;               // Disable JTAG, keep SWD

    /* PA0 */
    REG(GPIOA_CRL) &= ~(0xFU << 0);
    REG(GPIOA_CRL) |=  (0x8U << 0);
    REG(GPIOA_ODR) |=  (1U   << POWER);

    /* PA1 */
    REG(GPIOA_CRL) &= ~(0xFU << 4);
    REG(GPIOA_CRL) |=  (0x2U << 4);

    /* PB0 */
    REG(GPIOB_CRL) &= ~(0xFU  << 0);
    REG(GPIOB_CRL) |=  (0x4U  << 0);

    /* PB6/PB7 */
    REG(GPIOB_CRL) &= ~(0xFFU << 24);
    REG(GPIOB_CRL) |=  (0xFFU << 24);

    REG(STK_LOAD) = 72000 - 1;                 // SysTick = 1ms at 72MHz: LOAD = 72000 - 1 = 71999
    REG(STK_VAL)  = 0;
    REG(STK_CTRL) = CTRL_ENABLE | CTRL_TICKINT | CTRL_CLKSRC;
}

/* -----------------------------------------------------------------------------------
   I2C1 driver
   ----------------------------------------------------------------------------------- */
void i2c_init(void) {
	  REG(I2C_CR1)  &= ~CR1_PE;                   
    REG(I2C_CR2)   =  36;                       
    REG(I2C_CCR)   =  180;                     // Set SCL to 100kHz: 36MHz / (2 x 100kHz) = 180                  
    REG(I2C_TRISE) =  37;                      // Set maximum rise time: (36 x 1000ns) + 1 = 37 ticks                   
    REG(I2C_CR1)  |=  CR1_PE;                  
}

void i2c_start(void) {
	  REG(I2C_CR1) |= CR1_START;                   
    while (!(REG(I2C_SR1) & SR1_SB));
}

void i2c_stop(void) {
    REG(I2C_CR1) |= CR1_STOP;                    
    while (REG(I2C_SR2) & SR2_BUSY);            
}

void i2c_send(uint8_t b) {
    REG(I2C_DR) = b;                             
    while (!(REG(I2C_SR1) & SR1_BTF));           
}

void i2c_address(uint8_t addr) {
    REG(I2C_DR) = (uint8_t)(addr << 1);    
    while (!(REG(I2C_SR1) & SR1_ADDR));          
    (void)REG(I2C_SR1);                          
    (void)REG(I2C_SR2);                         
}

/* -----------------------------------------------------------------------------------
   OLED SSD1306 driver
   ----------------------------------------------------------------------------------- */
void oled_command(uint8_t cmd) {
    i2c_start();
    i2c_address(OLED_ADDR);
    i2c_send(OLED_CMD);      
    i2c_send(cmd);
    i2c_stop();
}

void oled_data(uint8_t dat) {
    i2c_start();
    i2c_address(OLED_ADDR);
    i2c_send(OLED_DAT);      
    i2c_send(dat);
    i2c_stop();
}

void oled_init(void) {
    delay(100);
    oled_command(0xAE);                       
    oled_command(0xD5); oled_command(0x80);   
    oled_command(0xA8); oled_command(0x3F);   
    oled_command(0xD3); oled_command(0x00);   
    oled_command(0x40);                     
    oled_command(0x8D); oled_command(0x14);  
    oled_command(0x20); oled_command(0x02);  
    oled_command(0xA1);                     
    oled_command(0xC8);                       
    oled_command(0xDA); oled_command(0x12);   
    oled_command(0x81); oled_command(0xCF);  
    oled_command(0xD9); oled_command(0xF1);   
    oled_command(0xDB); oled_command(0x40);   
    oled_command(0xA4);                      
    oled_command(0xA6);                    
    oled_command(0xAF);                    
}

void oled_set_cursor(uint32_t page, uint32_t col) {
    oled_command(0xB0 + page);      
    oled_command(col & 0x0F);       
    oled_command(0x10 | (col >> 4));
}

void oled_clear(void) {
    uint32_t p, c;
    for (p = 0; p < 8; p++) {
        oled_set_cursor(p, 0);
        for (c = 0; c < 128; c++) oled_data(0x00);
    }
}

/* -----------------------------------------------------------------------------------
   ASCII font 5x8 - characters 32 to 122
   ----------------------------------------------------------------------------------- */
uint8_t font[][5] = {
    {0x00,0x00,0x00,0x00,0x00},  // 32 space
    {0x00,0x00,0x5F,0x00,0x00},  // 33 !
    {0x00,0x07,0x00,0x07,0x00},  // 34 "
    {0x14,0x7F,0x14,0x7F,0x14},  // 35 #
    {0x24,0x2A,0x7F,0x2A,0x12},  // 36 $
    {0x23,0x13,0x08,0x64,0x62},  // 37 %
    {0x36,0x49,0x55,0x22,0x50},  // 38 &
    {0x00,0x05,0x03,0x00,0x00},  // 39 '
    {0x00,0x1C,0x22,0x41,0x00},  // 40 (
    {0x00,0x41,0x22,0x1C,0x00},  // 41 )
    {0x14,0x08,0x3E,0x08,0x14},  // 42 *
    {0x08,0x08,0x3E,0x08,0x08},  // 43 +
    {0x00,0x50,0x30,0x00,0x00},  // 44 ,
    {0x08,0x08,0x08,0x08,0x08},  // 45 -
    {0x00,0x60,0x60,0x00,0x00},  // 46 .
    {0x20,0x10,0x08,0x04,0x02},  // 47 /
    {0x3E,0x51,0x49,0x45,0x3E},  // 48 0
    {0x00,0x42,0x7F,0x40,0x00},  // 49 1
    {0x42,0x61,0x51,0x49,0x46},  // 50 2
    {0x21,0x41,0x45,0x4B,0x31},  // 51 3
    {0x18,0x14,0x12,0x7F,0x10},  // 52 4
    {0x27,0x45,0x45,0x45,0x39},  // 53 5
    {0x3C,0x4A,0x49,0x49,0x30},  // 54 6
    {0x01,0x71,0x09,0x05,0x03},  // 55 7
    {0x36,0x49,0x49,0x49,0x36},  // 56 8
    {0x06,0x49,0x49,0x29,0x1E},  // 57 9
    {0x00,0x36,0x36,0x00,0x00},  // 58 :
    {0x00,0x56,0x36,0x00,0x00},  // 59 ;
    {0x08,0x14,0x22,0x41,0x00},  // 60 <
    {0x14,0x14,0x14,0x14,0x14},  // 61 =
    {0x00,0x41,0x22,0x14,0x08},  // 62 >
    {0x02,0x01,0x51,0x09,0x06},  // 63 ?
    {0x32,0x49,0x79,0x41,0x3E},  // 64 @
    {0x7E,0x11,0x11,0x11,0x7E},  // 65 A
    {0x7F,0x49,0x49,0x49,0x36},  // 66 B
    {0x3E,0x41,0x41,0x41,0x22},  // 67 C
    {0x7F,0x41,0x41,0x22,0x1C},  // 68 D
    {0x7F,0x49,0x49,0x49,0x41},  // 69 E
    {0x7F,0x09,0x09,0x09,0x01},  // 70 F
    {0x3E,0x41,0x49,0x49,0x7A},  // 71 G
    {0x7F,0x08,0x08,0x08,0x7F},  // 72 H
    {0x00,0x41,0x7F,0x41,0x00},  // 73 I
    {0x20,0x40,0x41,0x3F,0x01},  // 74 J
    {0x7F,0x08,0x14,0x22,0x41},  // 75 K
    {0x7F,0x40,0x40,0x40,0x40},  // 76 L
    {0x7F,0x02,0x0C,0x02,0x7F},  // 77 M
    {0x7F,0x04,0x08,0x10,0x7F},  // 78 N
    {0x3E,0x41,0x41,0x41,0x3E},  // 79 O
    {0x7F,0x09,0x09,0x09,0x06},  // 80 P
    {0x3E,0x41,0x51,0x21,0x5E},  // 81 Q
    {0x7F,0x09,0x19,0x29,0x46},  // 82 R
    {0x46,0x49,0x49,0x49,0x31},  // 83 S
    {0x01,0x01,0x7F,0x01,0x01},  // 84 T
    {0x3F,0x40,0x40,0x40,0x3F},  // 85 U
    {0x1F,0x20,0x40,0x20,0x1F},  // 86 V
    {0x3F,0x40,0x38,0x40,0x3F},  // 87 W
    {0x63,0x14,0x08,0x14,0x63},  // 88 X
    {0x07,0x08,0x70,0x08,0x07},  // 89 Y
    {0x61,0x51,0x49,0x45,0x43},  // 90 Z
    {0x00,0x7F,0x41,0x00,0x00},  // 91 [
    {0x02,0x04,0x08,0x10,0x20},  // 92 backslash
    {0x00,0x41,0x7F,0x00,0x00},  // 93 ]
    {0x04,0x02,0x01,0x02,0x04},  // 94 ^
    {0x40,0x40,0x40,0x40,0x40},  // 95 _
    {0x00,0x01,0x02,0x04,0x00},  // 96 `
    {0x20,0x54,0x54,0x54,0x78},  // 97  a
    {0x7F,0x48,0x44,0x44,0x38},  // 98  b
    {0x38,0x44,0x44,0x44,0x20},  // 99  c
    {0x38,0x44,0x44,0x48,0x7F},  // 100 d
    {0x38,0x54,0x54,0x54,0x18},  // 101 e
    {0x08,0x7E,0x09,0x01,0x02},  // 102 f
    {0x0C,0x52,0x52,0x52,0x3E},  // 103 g
    {0x7F,0x08,0x04,0x04,0x78},  // 104 h
    {0x00,0x44,0x7D,0x40,0x00},  // 105 i
    {0x20,0x40,0x44,0x3D,0x00},  // 106 j
    {0x7F,0x10,0x28,0x44,0x00},  // 107 k
    {0x00,0x41,0x7F,0x40,0x00},  // 108 l
    {0x7C,0x04,0x18,0x04,0x78},  // 109 m
    {0x7C,0x08,0x04,0x04,0x78},  // 110 n
    {0x38,0x44,0x44,0x44,0x38},  // 111 o
    {0x7C,0x14,0x14,0x14,0x08},  // 112 p
    {0x08,0x14,0x14,0x18,0x7C},  // 113 q
    {0x7C,0x08,0x04,0x04,0x08},  // 114 r
    {0x48,0x54,0x54,0x54,0x20},  // 115 s
    {0x04,0x3F,0x44,0x40,0x20},  // 116 t
    {0x3C,0x40,0x40,0x20,0x7C},  // 117 u
    {0x1C,0x20,0x40,0x20,0x1C},  // 118 v
    {0x3C,0x40,0x30,0x40,0x3C},  // 119 w
    {0x44,0x28,0x10,0x28,0x44},  // 120 x
    {0x0C,0x50,0x50,0x50,0x3C},  // 121 y
    {0x44,0x64,0x54,0x4C,0x44},  // 122 z
};
#define FONT_FIRST  32       
#define FONT_SIZE   91        

void oled_char(char c) {
    uint32_t i, idx = (int32_t)c - FONT_FIRST;
    if (idx < 0 || idx >= FONT_SIZE) idx = 0;    
    for (i = 0; i < 5; i++) oled_data(font[idx][i]);
    oled_data(0x00);                              
}

void oled_string(char *str) {
    while (*str) oled_char(*str++);
}

/* -----------------------------------------------------------------------------------
   UNIFEI logo
   ----------------------------------------------------------------------------------- */
void oled_logo_unifei(uint32_t page, uint32_t col) {
    uint8_t p0[15] = {
        0x30,0xF8,0x18,0x0E,0x07,
        0xF3,0x92,0xD2,0xB2,0x9B,
        0x07,0x0E,0x18,0xF8,0x30
    };
    uint8_t p1[15] = {
        0x06,0x0F,0x0C,0x38,0x70,
        0x67,0x2C,0x26,0x25,0x64,
        0x70,0x38,0x0C,0x0F,0x06
    };
    uint32_t i;
    oled_set_cursor(page, col);
    for (i = 0; i < 15; i++) oled_data(p0[i]);
    oled_set_cursor(page + 1, col);
    for (i = 0; i < 15; i++) oled_data(p1[i]);
}

/* -----------------------------------------------------------------------------------
   UoM logo
   ----------------------------------------------------------------------------------- */
void oled_logo_manchester(uint32_t page, uint32_t col) {
    uint8_t p0[15] = {
        0xFF,0x01,0xFD,0xFD,0x0D,
        0x31,0xC1,0x01,0xC1,0x31,
        0x0D,0xFD,0xFD,0x01,0xFF
    };
    uint8_t p1[15] = {
        0xFF,0x80,0xBF,0xBF,0x80,
        0x80,0x80,0x83,0x80,0x80,
        0x80,0xBF,0xBF,0x80,0xFF
    };
    uint32_t i;
    oled_set_cursor(page, col);
    for (i = 0; i < 15; i++) oled_data(p0[i]);
    oled_set_cursor(page + 1, col);
    for (i = 0; i < 15; i++) oled_data(p1[i]);
}

/* -----------------------------------------------------------------------------------
   Display output
   ----------------------------------------------------------------------------------- */
void oled_boot(void) {
    oled_clear();
    oled_logo_unifei(0, 0);
    oled_logo_manchester(0, 112);
    oled_set_cursor(1, 21); oled_string("UNIFEI and UoM");
    oled_set_cursor(3, 20); oled_string("Radio Frequency ");
    oled_set_cursor(5, 20); oled_string("  Cyber Threat  ");
    oled_set_cursor(7, 20); oled_string("Detection Device");
    delay(2000);
    oled_clear();
}

void update_display(void) {
    if (!powered) return;
    oled_logo_unifei(0, 94);
    oled_logo_manchester(0, 112);
    oled_set_cursor(2, 0);
    switch (rf_state) {
    case JAMMING:
        oled_string("JAMMING!        ");
        oled_set_cursor(4, 0); oled_string("RF Interference ");
        break;
    case FAULT:
        oled_string("NO Signal       ");
        oled_set_cursor(4, 0); oled_string("Error device    ");
        break;
    case NORMAL:
        oled_string("Normal          ");
        oled_set_cursor(4, 0); oled_string("RF Signal OK    ");
        break;
    default:
        oled_string("Waiting...      ");
        oled_set_cursor(4, 0); oled_string("                ");
        break;
    }
    oled_set_cursor(7, 0); oled_string("By UNIFEI and UoM");
}

/* -----------------------------------------------------------------------------------
   Buzzer control
   ----------------------------------------------------------------------------------- */
void update_buzzer(void) {
    if (powered && rf_state == JAMMING)
        REG(GPIOA_ODR) |=  (1U << BUZZER);     // PA1 high: buzzer on 
    else
        REG(GPIOA_ODR) &= ~(1U << BUZZER);     // PA1 low:  buzzer off
}

/* -----------------------------------------------------------------------------------
   RF signal classification
   ----------------------------------------------------------------------------------- */
uint32_t count_ones(void) {
    uint32_t total = 0, i, b;
    for (i = 0; i < WINDOW_BYTES; i++)
        for (b = 0; b < 8; b++)
            if (window[i] & (1U << b)) total++;
    return total;
}

uint32_t classify(void) {
    uint32_t ones = count_ones();
    if (ones == WINDOW_BITS) return JAMMING;   // all bits 1: interference
    if (ones == 0)           return FAULT;     // all bits 0: no signal   
    return NORMAL;                             // mixed bits: valid signal
}

void accumulate_byte(uint8_t byte) {
    window[window_idx] = byte;
    window_idx++;
    if (window_idx == WINDOW_BYTES) {
        window_idx = 0;
        rf_state   = classify();
        update_display();
        update_buzzer();
    }
}

/* -----------------------------------------------------------------------------------
   RF hardware reading
   ----------------------------------------------------------------------------------- */
void read_rf(void) {
    bit_pos++;
    if (REG(GPIOB_IDR) & (1U << RF_IN))        // PB0 = 1 
        current_byte |=  (1U << bit_pos);
    else                                       // PB0 = 0
        current_byte &= ~(1U << bit_pos);
    if (bit_pos == 8) {
        accumulate_byte(current_byte);
        current_byte = 0;
        bit_pos      = 0;
    }
}

/* -----------------------------------------------------------------------------------
   Power button
   ----------------------------------------------------------------------------------- */
uint32_t get_button_state(void) {
    if (REG(GPIOA_IDR) & (1U << POWER))
        return 0;                              // PA0 high: button released 
    return 1;                                  // PA0 low:  button pressed 
}

void check_button(void) {
    if (!get_button_state()) return;
    delay(50);
    if (!get_button_state()) return;

  	powered = !powered;

    if (powered) {
        window_idx   = 0;
        rf_state     = -1;
        current_byte = 0;
        bit_pos      = 0;
        REG(GPIOA_ODR) &= ~(1U << BUZZER);
        oled_boot();
        update_display();
    } else {
        REG(GPIOA_ODR) &= ~(1U << BUZZER);
        oled_clear();
        oled_logo_unifei(0, 94);
        oled_logo_manchester(0, 112);
        oled_set_cursor(3, 0); oled_string("System OFF      ");
        oled_set_cursor(5, 0); oled_string("Power to turn ON");
    }
    while (get_button_state()) delay(10);
}

/* -----------------------------------------------------------------------------------
   Main
   ----------------------------------------------------------------------------------- */
int main(void) {
    hardware_init();
    i2c_init();
    oled_init();
    oled_boot();
    update_display();

    while(1) {
        check_button();
        if (powered) {
            read_rf();
            delay(20);
        }
    }
}