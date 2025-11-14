#include "error_handler.h"
#include <stdio.h>
#include "cJSON.h"
#include "utils.h"
#include <stdlib.h>
void print_active_errors(uint32_t error_mask, const char *json_path) {
    char *json_text = read_file(json_path);
    if (!json_text) return;

    cJSON *root = cJSON_Parse(json_text);
    cJSON *enums = cJSON_GetObjectItem(root, "enums");
    cJSON *error_codes = cJSON_GetObjectItem(enums, "error_codes");
    cJSON *values = cJSON_GetObjectItem(error_codes, "values");

    printf("=== Active Error Codes ===\n");
    for (int i = 0; i < 32; i++) {
        if ((error_mask >> i) & 1) {
            cJSON *msg = cJSON_GetObjectItem(values, utils_int_to_str(i));
            if (msg)
                printf("Bit%-2d: %s\n", i, msg->valuestring);
        }
    }

    cJSON_Delete(root);
    free(json_text);
}
