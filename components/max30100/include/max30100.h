/*
MIT License

Copyright (c) 2017 Raivis Strogonovs (https://morf.lv)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

/*
Translation to C and ESP32 version

Author: Angelo Elias Dalzotto (150633@upf.br)
GEPID - Grupo de Pesquisa em Cultura Digital (http://gepid.upf.br/)
Universidade de Passo Fundo (http://www.upf.br/)
*/

#ifndef MAX30100_H
#define MAX30100_H

#include <math.h>
#include "driver/i2c.h"
#include "freertos/task.h"

/*----------------------------------------------*/
/* Config defines, you can tailor to your needs */
/*----------------------------------------------*/

/* MAX30100 parameters */
#define MAX30100_DEFAULT_OPERATING_MODE            MAX30100_MODE_SPO2_HR
/*!!!IMPORTANT
 * You can't just throw these two values at random.
 * Check Check table 8 in datasheet on page 19.
 * 100hz + 1600us is max for that resolution
 */

#define MAX30100_DEFAULT_SAMPLING_RATE        MAX30100_SAMPLING_RATE_100HZ
#define MAX30100_DEFAULT_LED_PULSE_WIDTH      MAX30100_PULSE_WIDTH_1600US_ADC_16

#define MAX30100_DEFAULT_IR_LED_CURRENT       MAX30100_LED_CURRENT_50MA
#define MAX30100_STARTING_RED_LED_CURRENT     MAX30100_LED_CURRENT_27_1MA

/* Adjust RED LED current balancing*/
#define MAX30100_MAGIC_ACCEPTABLE_INTENSITY_DIFF         65000
#define MAX30100_RED_LED_CURRENT_ADJUSTMENT_MS           500

/* SaO2 parameters */
#define MAX30100_RESET_SPO2_EVERY_N_PULSES     4

/* Filter parameters */
#define MAX30100_ALPHA                    0.95  //dc filter alpha value
#define MAX30100_MEAN_FILTER_SIZE        15

/* Pulse detection parameters */
//300 is good for finger, but for wrist you need like 20,
//and there is shitloads of noise
#define MAX30100_PULSE_MIN_THRESHOLD         300
#define MAX30100_PULSE_MAX_THRESHOLD         2000
#define MAX30100_PULSE_GO_DOWN_THRESHOLD     1

#define MAX30100_PULSE_BPM_SAMPLE_SIZE       10 //Moving average size





/* Enums, data structures and typdefs. DO NOT EDIT */
typedef struct _max30100_data_t {
  bool pulse_detected;
  float heart_bpm;

  float ir_cardiogram;

  float ir_dc_value;
  float red_dc_value;

  float spO2;

  uint32_t last_beat_threshold;

  float dc_filtered_red;
  float dc_filtered_ir;
} max30100_data_t;

typedef enum _pulse_state_machine {
    MAX30100_PULSE_IDLE,
    MAX30100_PULSE_TRACE_UP,
    MAX30100_PULSE_TRACE_DOWN
} pulse_state_machine;

typedef struct _max30100_fifo_t {
  uint16_t raw_ir;
  uint16_t raw_red;
} max30100_fifo_t;

typedef struct _max30100_dc_filter_t {
  float w;
  float result;
} max30100_dc_filter_t;

typedef struct _max30100_butterworth_filter_t
{
  float v[2];
  float result;
} max30100_butterworth_filter_t;

typedef struct _max30100_mean_diff_filter_t
{
  float values[MAX30100_MEAN_FILTER_SIZE];
  uint8_t index;
  float sum;
  uint8_t count;
} max30100_mean_diff_filter_t;

/* MAX30100 register and bit defines, DO NOT EDIT */
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

typedef enum _max30100_mode_t {
    MAX30100_MODE_HR_ONLY                 = 0x02,
    MAX30100_MODE_SPO2_HR                 = 0x03
} max30100_mode_t;

//Bit defines SpO2 register
#define MAX30100_SPO2_HI_RES_EN           (1 << 6)
typedef enum SamplingRate {
    MAX30100_SAMPLING_RATE_50HZ           = 0x00,
    MAX30100_SAMPLING_RATE_100HZ          = 0x01,
    MAX30100_SAMPLING_RATE_167HZ          = 0x02,
    MAX30100_SAMPLING_RATE_200HZ          = 0x03,
    MAX30100_SAMPLING_RATE_400HZ          = 0x04,
    MAX30100_SAMPLING_RATE_600HZ          = 0x05,
    MAX30100_SAMPLING_RATE_800HZ          = 0x06,
    MAX30100_SAMPLING_RATE_1000HZ         = 0x07
} max30100_sampling_rate_t;

typedef enum LEDPulseWidth {
    MAX30100_PULSE_WIDTH_200US_ADC_13     = 0x00,
    MAX30100_PULSE_WIDTH_400US_ADC_14     = 0x01,
    MAX30100_PULSE_WIDTH_800US_ADC_15     = 0x02,
    MAX30100_PULSE_WIDTH_1600US_ADC_16    = 0x03,
} max30100_pulse_width_t;

typedef enum LEDCurrent {
    MAX30100_LED_CURRENT_0MA              = 0x00,
    MAX30100_LED_CURRENT_4_4MA            = 0x01,
    MAX30100_LED_CURRENT_7_6MA            = 0x02,
    MAX30100_LED_CURRENT_11MA             = 0x03,
    MAX30100_LED_CURRENT_14_2MA           = 0x04,
    MAX30100_LED_CURRENT_17_4MA           = 0x05,
    MAX30100_LED_CURRENT_20_8MA           = 0x06,
    MAX30100_LED_CURRENT_24MA             = 0x07,
    MAX30100_LED_CURRENT_27_1MA           = 0x08,
    MAX30100_LED_CURRENT_30_6MA           = 0x09,
    MAX30100_LED_CURRENT_33_8MA           = 0x0A,
    MAX30100_LED_CURRENT_37MA             = 0x0B,
    MAX30100_LED_CURRENT_40_2MA           = 0x0C,
    MAX30100_LED_CURRENT_43_6MA           = 0x0D,
    MAX30100_LED_CURRENT_46_8MA           = 0x0E,
    MAX30100_LED_CURRENT_50MA             = 0x0F
} max30100_current_t;

//"Class"
typedef struct _max30100_config_t {
//PRIVATE
    i2c_port_t i2c_num;
    //Incluir mutex

    bool debug;

    uint8_t red_current;
    float last_red_current_check;

    uint8_t current_pulse_detector_state;
    float current_bpm;
    float values_bpm[MAX30100_PULSE_BPM_SAMPLE_SIZE];
    float values_bpm_sum;
    uint8_t values_bpm_count;
    uint8_t bpm_index;
    uint32_t last_beat_threshold;

    max30100_fifo_t prev_fifo;

    max30100_dc_filter_t dc_filter_ir;
    max30100_dc_filter_t dc_filter_red;
    max30100_butterworth_filter_t lpb_filter_ir;
    max30100_mean_diff_filter_t mean_diff_ir;

    float ir_ac_sq_sum;
    float red_ac_sq_sum;
    uint16_t samples_recorded;
    uint16_t pulses_detected;
    float current_spO2;

    max30100_current_t ir_current;
} max30100_config_t;

//PUBLIC
esp_err_t max30100_init( max30100_config_t* this,
                         i2c_port_t i2c_num,
                         max30100_mode_t mode,
                         max30100_sampling_rate_t sampling_rate,
                         max30100_pulse_width_t pulse_width,
                         max30100_current_t ir_current,
                         bool high_res_mode,
                         bool debug );

esp_err_t max30100_update(max30100_config_t* this, max30100_data_t* data);
esp_err_t max30100_set_mode(max30100_config_t* this, max30100_mode_t mode);
esp_err_t max30100_set_high_res(max30100_config_t* this, bool enabled);

esp_err_t max330100_read_temperature( max30100_config_t* this,
                                      float* temperature );

esp_err_t max30100_read_fifo(max30100_config_t* this, max30100_fifo_t* fifo);
esp_err_t max30100_print_registers(max30100_config_t* this);

esp_err_t max30100_set_led_current( max30100_config_t* this,
                                    uint8_t red_current,
                                    uint8_t ir_current );

esp_err_t max30100_set_pulse_width( max30100_config_t* this,
                                    max30100_pulse_width_t pw );

esp_err_t max30100_set_sampling_rate( max30100_config_t* this,
                                      max30100_sampling_rate_t rate );

max30100_dc_filter_t max30100_dc_removal( float x,
                                          float prev_w,
                                          float alpha );

float max30100_mean_diff(max30100_config_t* this, float M);
void max30100_lpb_filter(max30100_config_t* this, float x);

//PRIVATE
//Don't call these functions!!
bool max30100_detect_pulse(max30100_config_t* this, float sensor_value);

esp_err_t max30100_balance_intensities( max30100_config_t* this,
                                        float red_dc,
                                        float ir_dc );

esp_err_t max30100_write_register( max30100_config_t* this,
                                   uint8_t address,
                                   uint8_t val );

esp_err_t max30100_read_register( max30100_config_t* this,
                                  uint8_t address,
                                  uint8_t* reg );

esp_err_t max30100_read_from( max30100_config_t* this,
                              uint8_t address,
                              uint8_t* data,
                              uint8_t size );

#endif
