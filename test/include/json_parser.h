#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char *key;
    char *value;
} JsonPair;

typedef struct {
    char model_name[64];
    uint8_t slave_addr;
    int baudrate;
    char endianness[8];
} CommunicationConfig;

typedef struct {
    uint16_t addr;
    uint8_t len;
    char type[8];
    float scale;
    char tag[64];
    char enum_name[64];
} TagEntry;

typedef struct {
    CommunicationConfig comm;
    TagEntry error_message_tag;
    TagEntry warning_code_tag;
} InverterConfig;

bool load_inverter_config(const char *filename, InverterConfig *cfg);
const char* get_error_description(uint32_t bitmask, const char *json_path);

/**
 * Khởi tạo tất cả các mảng tham số từ file JSON GW20K-ET-L-G10_v1_0_0.json
 * Cập nhật các biến extern trong device_manager.h
 * @param json_filename: đường dẫn tới file JSON
 * @return true nếu thành công, false nếu lỗi
 */
bool init_all_parameters_from_json(const char *json_filename);

/**
 * Giải phóng bộ nhớ của các mảng tham số
 */
void cleanup_parameters(void);

#endif