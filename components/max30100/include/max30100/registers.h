#include "max30100.h"

/* MAX30100 register and bit defines */
#define MAX30100_DEVICE                   0x57

//Part ID Registers
#define MAX30100_REV_ID                   0xFE
#define MAX30100_PART_ID                  0xFF

//status registers
#define MAX30100_INT_STATUS               0x00
#define MAX30100_INT_ENABLE               0x01

//Fifo registers
#define MAX30100_FIFO_WRITE               0x02
#define MAX30100_FIFO_OVERFLOW_COUNTER    0x03
#define MAX30100_FIFO_READ                0x04
#define MAX30100_FIFO_DATA                0x05

//Config registers
#define MAX30100_MODE_CONF                0x06
#define MAX30100_SPO2_CONF                0x07
#define MAX30100_LED_CONF                 0x09

//Temperature registers
#define MAX30100_TEMP_INT                 0x16
#define MAX30100_TEMP_FRACTION            0x17

//Bit defines MODE Regsiter
#define MAX30100_MODE_SHDN                (1<<7)
#define MAX30100_MODE_RESET               (1<<6)
#define MAX30100_MODE_TEMP_EN             (1<<3)

//Bit defines SpO2 register
#define MAX30100_SPO2_HI_RES_EN           (1<<6)

/* Private functions declarations */
bool max30100_detect_pulse(max30100_config_t* this, float sensor_value);
esp_err_t max30100_balance_intensities(max30100_config_t* this, float red_dc, float ir_dc);
esp_err_t max30100_write_register(max30100_config_t* this, uint8_t address, uint8_t val);
esp_err_t max30100_read_register(max30100_config_t* this, uint8_t address, uint8_t* reg);
esp_err_t max30100_read_from(max30100_config_t* this, uint8_t address, uint8_t* data, uint8_t size );
esp_err_t max30100_read_fifo(max30100_config_t* this, max30100_fifo_t* fifo);
max30100_dc_filter_t max30100_dc_removal(float x, float prev_w, float alpha);
float max30100_mean_diff(max30100_config_t* this, float M);
void max30100_lpb_filter(max30100_config_t* this, float x);