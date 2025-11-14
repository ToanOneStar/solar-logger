#include "device_manager.h"
#include <stdlib.h>
#include <string.h>

// Định nghĩa các biến extern cho các mảng tham số modbus
int num_param_init = 0;
mb_parameter_descriptor_t *initialization_parameters = NULL;

int num_param_runtime = 0;
mb_parameter_descriptor_t *runtime_data_parameters = NULL;

int num_param_battery = 0;
mb_parameter_descriptor_t *battery_data_parameters = NULL;

int num_param_accumulation = 0;
mb_parameter_descriptor_t *energy_accumulation_parameters = NULL;

int num_param_comsumption = 0;
mb_parameter_descriptor_t *energy_consumption_parameters = NULL;

int num_param_time = 0;
mb_parameter_descriptor_t *time_data_parameters = NULL;

