/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "detection_responder.h"
#include "hx_drv_tflm.h"

#include "hx_drv_tflm.h"
#include "synopsys_wei_delay.h"
#include "stdio.h"

#define accel_scale 10

// uint8_t string_buf[100] = "test\n";

typedef struct
{
	uint8_t symbol;
	uint32_t int_part;
	uint32_t frac_part;
} accel_type;


// This dummy implementation writes person and no person scores to the error
// console. Real applications will want to take some custom action instead, and
// should implement their own versions of this function.
void RespondToDetection(tflite::ErrorReporter* error_reporter,int8_t person_score, int8_t no_person_score) {

  int32_t int_buf;
	accel_type accel_x, accel_y, accel_z;
	uint32_t msec_cnt = 0;
	uint32_t sec_cnt = 0;

  

  // hx_drv_uart_print("Testing: %d.%d sec\n", sec_cnt, msec_cnt / 100);

  uint32_t available_count = 0;
  float x, y, z;
  available_count = hx_drv_accelerometer_available_count();				
  // hx_drv_uart_print("Accel get FIFO: %d\n", available_count);
  for (int i = 0; i < available_count; i++){
    hx_drv_accelerometer_receive(&x, &y, &z);
  }

  int_buf = x * accel_scale; //scale value
  if(int_buf < 0){
    int_buf = int_buf * -1;
    accel_x.symbol = '-';
  }
  else{
    accel_x.symbol = '+';
  }
  accel_x.int_part = int_buf / accel_scale;
  accel_x.frac_part = int_buf % accel_scale;


  int_buf = y * accel_scale; //scale value
  if(int_buf < 0){
    int_buf = int_buf * -1;
    accel_y.symbol = '-';
  }else{
    accel_y.symbol = '+';
  }
  accel_y.int_part = int_buf / accel_scale;
  accel_y.frac_part = int_buf % accel_scale;


  int_buf = z * accel_scale; //scale value
  if(int_buf < 0){
    int_buf = int_buf * -1;
    accel_z.symbol = '-';
  }
  else{
    accel_z.symbol = '+';
  }
  accel_z.int_part = int_buf / accel_scale;
  accel_z.frac_part = int_buf % accel_scale;

  // sprintf(string_buf, "%c%1d.%1d | %c%1d.%1d | %c%1d.%1d G\n", 
  //     accel_x.symbol, accel_x.int_part, accel_x.frac_part, 
  //     accel_y.symbol, accel_y.int_part, accel_y.frac_part, 
  //     accel_z.symbol, accel_z.int_part, accel_z.frac_part);
  // hx_drv_uart_print(string_buf);

  if (accel_x.int_part>=2||accel_y.int_part>=2||accel_z.int_part>=2)
  {
    if(person_score > no_person_score) {
      hx_drv_uart_print("合理急殺\n");
      hx_drv_led_off(HX_DRV_LED_GREEN);
    }else{
      hx_drv_uart_print("惡意急殺\n");
      hx_drv_led_on(HX_DRV_LED_GREEN);
    }
  }
  hx_drv_led_off(HX_DRV_LED_GREEN);

  TF_LITE_REPORT_ERROR(error_reporter, "person score:%d no person score %d",
                       person_score, no_person_score);
}
