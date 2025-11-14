#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdint.h>
#include "json_parser.h"

void print_active_errors(uint32_t error_mask, const char *json_path);

#endif