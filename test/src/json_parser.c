#include "json_parser.h"
#include "device_manager.h"
#include "utils.h"
#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Chuyển đổi string param_type từ JSON thành mb_descr_type_t enum
 */
static mb_descr_type_t json_str_to_param_type(const char *type_str)
{
    if (!type_str)
        return PARAM_TYPE_U16;

    if (strcmp(type_str, "u8") == 0)
        return PARAM_TYPE_U8;
    if (strcmp(type_str, "u16") == 0)
        return PARAM_TYPE_U16;
    if (strcmp(type_str, "u32") == 0)
        return PARAM_TYPE_U32;
    if (strcmp(type_str, "s8") == 0)
        return PARAM_TYPE_I8_A;
    if (strcmp(type_str, "s16") == 0)
        return PARAM_TYPE_I16_AB;
    if (strcmp(type_str, "s32") == 0)
        return PARAM_TYPE_I32_ABCD;
    if (strcmp(type_str, "float") == 0)
        return PARAM_TYPE_FLOAT_ABCD;
    if (strcmp(type_str, "ascii") == 0)
        return PARAM_TYPE_ASCII;

    return PARAM_TYPE_U16;
}

/**
 * Chuyển đổi string mb_param_type từ JSON thành mb_param_type_t enum
 */
static mb_param_type_t json_str_to_mb_param_type(const char *type_str)
{
    if (!type_str)
        return MB_PARAM_INPUT;

    if (strcmp(type_str, "HOLDING") == 0)
        return MB_PARAM_HOLDING;
    if (strcmp(type_str, "INPUT") == 0)
        return MB_PARAM_INPUT;
    if (strcmp(type_str, "COIL") == 0)
        return MB_PARAM_COIL;
    if (strcmp(type_str, "DISCRETE") == 0)
        return MB_PARAM_DISCRETE;

    return MB_PARAM_INPUT;
}

/**
 * Chuyển đổi string param_size từ JSON thành mb_descr_size_t enum
 */
static mb_descr_size_t json_size_to_param_size(int size_bytes)
{
    switch (size_bytes)
    {
    case 1:
        return PARAM_SIZE_U8;
    case 2:
        return PARAM_SIZE_U16;
    case 4:
        return PARAM_SIZE_U32;
    case 8:
        return PARAM_SIZE_U64;
    default:
        return PARAM_SIZE_U16;
    }
}

/**
 * Chuyển đổi string access từ JSON thành mb_param_perms_t enum
 */
static mb_param_perms_t json_str_to_access_perm(const char *access_str)
{
    if (!access_str)
        return PAR_PERMS_READ;

    if (strcmp(access_str, "ro") == 0)
        return PAR_PERMS_READ;
    if (strcmp(access_str, "rw") == 0)
        return PAR_PERMS_READ_WRITE;
    if (strcmp(access_str, "wo") == 0)
        return PAR_PERMS_WRITE;

    return PAR_PERMS_READ;
}

/**
 * Chuyển đổi string param_type thành hash (cid) - sử dụng simple hash
 */
static uint16_t hash_cid(const char *cid_str)
{
    if (!cid_str)
        return 0;

    uint16_t hash = 0;
    for (int i = 0; i < strlen(cid_str) && i < 16; i++)
    {
        hash = hash * 31 + cid_str[i];
    }
    return hash & 0xFFFF;
}

/**
 * Đọc file JSON cấu hình inverter và load vào cấu trúc InverterConfig
 */
bool load_inverter_config(const char *filename, InverterConfig *cfg)
{
    if (!filename || !cfg)
        return false;

    char *json_text = read_file(filename);
    if (!json_text)
    {
        printf(" Cannot open file: %s\n", filename);
        return false;
    }

    cJSON *root = cJSON_Parse(json_text);
    if (!root)
    {
        printf(" JSON parse error\n");
        free(json_text);
        return false;
    }

    // ---- communication ----
    cJSON *comm = cJSON_GetObjectItem(root, "communication");
    if (comm)
    {
        cJSON *addr = cJSON_GetObjectItem(comm, "address");
        cJSON *baud = cJSON_GetObjectItem(comm, "baudrate");
        cJSON *endian = cJSON_GetObjectItem(comm, "endianness");

        cfg->comm.slave_addr = addr ? addr->valueint : 1;
        cfg->comm.baudrate = baud ? baud->valueint : 9600;
        strcpy(cfg->comm.endianness, endian ? endian->valuestring : "msb");
    }

    // ---- runtime_data -> tìm tag error_message ----
    cJSON *groups = cJSON_GetObjectItem(root, "register_groups");
    cJSON *runtime = cJSON_GetObjectItem(groups, "runtime_data");
    if (runtime)
    {
        cJSON *tags = cJSON_GetObjectItem(runtime, "tags");
        cJSON *tag;
        cJSON_ArrayForEach(tag, tags)
        {
            cJSON *tag_name = cJSON_GetObjectItem(tag, "tag");
            if (strcmp(tag_name->valuestring, "error_message") == 0)
            {
                cfg->error_message_tag.addr = cJSON_GetObjectItem(tag, "addr")->valueint;
                cfg->error_message_tag.len = cJSON_GetObjectItem(tag, "len")->valueint;
                strcpy(cfg->error_message_tag.enum_name,
                       cJSON_GetObjectItem(tag, "enum")->valuestring);
            }
            if (strcmp(tag_name->valuestring, "warning_code") == 0)
            {
                cfg->warning_code_tag.addr = cJSON_GetObjectItem(tag, "addr")->valueint;
                cfg->warning_code_tag.len = cJSON_GetObjectItem(tag, "len")->valueint;
            }
        }
    }

    cJSON_Delete(root);
    free(json_text);
    return true;
}

/**
 * Trả về mô tả lỗi tương ứng với bitmask error_code dựa trên JSON config
 * (đọc từ phần "enums" -> "error_codes" -> "values")
 */
const char *get_error_description(uint32_t bitmask, const char *json_path)
{
    static char description[2048];
    description[0] = '\0';

    char *json_text = read_file(json_path);
    if (!json_text)
        return NULL;

    cJSON *root = cJSON_Parse(json_text);
    if (!root)
    {
        free(json_text);
        return NULL;
    }

    cJSON *enums = cJSON_GetObjectItem(root, "enums");
    if (!enums)
    {
        cJSON_Delete(root);
        free(json_text);
        return NULL;
    }

    cJSON *error_codes = cJSON_GetObjectItem(enums, "error_codes");
    if (!error_codes)
    {
        cJSON_Delete(root);
        free(json_text);
        return NULL;
    }

    cJSON *values = cJSON_GetObjectItem(error_codes, "values");
    if (!values)
    {
        cJSON_Delete(root);
        free(json_text);
        return NULL;
    }

    strcat(description, "Active Errors:\n");
    for (int i = 0; i < 32; i++)
    {
        if ((bitmask >> i) & 1)
        {
            cJSON *msg = cJSON_GetObjectItem(values, utils_int_to_str(i));
            if (msg && msg->valuestring)
            {
                strcat(description, "- ");
                strcat(description, msg->valuestring);
                strcat(description, "\n");
            }
        }
    }

    cJSON_Delete(root);
    free(json_text);
    return description;
}

/**
 * Khởi tạo mảng mb_parameter_descriptor_t từ register group trong JSON
 * Trả về số phần tử được tạo, hoặc 0 nếu lỗi
 */
static int init_parameter_array_from_group(
    const cJSON *group,
    const char *group_name,
    mb_parameter_descriptor_t *param_array,
    int max_elements,
    uint8_t mb_slave_addr)
{

    if (!group || !param_array || max_elements <= 0)
        return 0;

    cJSON *cids = cJSON_GetObjectItem(group, "cids");
    if (!cids || !cJSON_IsArray(cids))
    {
        printf("  Warning: No 'cids' array found in group '%s'\n", group_name);
        return 0;
    }

    int count = 0;
    cJSON *cid_item = NULL;

    cJSON_ArrayForEach(cid_item, cids)
    {
        if (count >= max_elements)
        {
            printf("  Warning: Max elements (%d) reached for group '%s'\n", max_elements, group_name);
            break;
        }

        // Lấy dữ liệu từ JSON
        cJSON *cid_str = cJSON_GetObjectItem(cid_item, "cid");
        cJSON *param_key_str = cJSON_GetObjectItem(cid_item, "param_key");
        cJSON *mb_reg_start = cJSON_GetObjectItem(cid_item, "mb_reg_start");
        cJSON *mb_size = cJSON_GetObjectItem(cid_item, "mb_size");
        cJSON *param_type_str = cJSON_GetObjectItem(cid_item, "param_type");
        cJSON *param_size = cJSON_GetObjectItem(cid_item, "param_size");
        cJSON *mb_param_type = cJSON_GetObjectItem(cid_item, "mb_param_type");
        cJSON *param_units = cJSON_GetObjectItem(cid_item, "param_units");
        cJSON *access_str = cJSON_GetObjectItem(cid_item, "access");
        cJSON *range = cJSON_GetObjectItem(cid_item, "range");

        // Khởi tạo struct
        mb_parameter_descriptor_t *p = &param_array[count];
        memset(p, 0, sizeof(mb_parameter_descriptor_t));

        // Gán giá trị
        //p->cid = hash_cid(cid_str ? cid_str->valuestring : "");
        if (cid_str && cid_str->valuestring)
        {
            size_t l = strlen(cid_str->valuestring) + 1;
            char *k = (char *)malloc(l);
            if (k)
                memcpy(k, cid_str->valuestring, l);
            p->cid = k;
        }
        else
        {
            p->cid = NULL;
        }

        if (param_key_str && param_key_str->valuestring)
        {
            size_t l = strlen(param_key_str->valuestring) + 1;
            char *k = (char *)malloc(l);
            if (k)
                memcpy(k, param_key_str->valuestring, l);
            p->param_key = k;
        }
        else
        {
            p->param_key = NULL;
        }

        if (param_units && param_units->valuestring)
        {
            size_t l2 = strlen(param_units->valuestring) + 1;
            char *u = (char *)malloc(l2);
            if (u)
                memcpy(u, param_units->valuestring, l2);
            p->param_units = u;
        }
        else
        {
            p->param_units = NULL;
        }
        p->mb_slave_addr = mb_slave_addr;
        p->mb_param_type = json_str_to_mb_param_type(mb_param_type ? mb_param_type->valuestring : "INPUT");
        p->mb_reg_start = mb_reg_start ? mb_reg_start->valueint : 0;
        p->mb_size = mb_size ? mb_size->valueint : 1;
        p->param_offset = 0; // Sẽ được xác định sau
        p->param_type = json_str_to_param_type(param_type_str ? param_type_str->valuestring : "u16");
        p->param_size = json_size_to_param_size(param_size ? param_size->valueint : 2);
        p->access = json_str_to_access_perm(access_str ? access_str->valuestring : "ro");

        // Xử lý range (min, max, scale)
        if (range && cJSON_IsObject(range))
        {
            cJSON *min = cJSON_GetObjectItem(range, "min");
            cJSON *max = cJSON_GetObjectItem(range, "max");
            cJSON *scale = cJSON_GetObjectItem(range, "scale");

            if (min)
                p->param_opts.min = min->valueint;
            if (max)
                p->param_opts.max = max->valueint;
            if (scale)
                p->param_opts.step = (int)(scale->valuedouble); // Lưu dưới dạng int
        }

        count++;
    }

    return count;
}

/**
 * Khởi tạo tất cả các mảng tham số từ file JSON
 * Cập nhật các extern variables và trả về true nếu thành công
 */
bool init_all_parameters_from_json(const char *json_filename)
{
    if (!json_filename)
    {
        printf("  Error: JSON filename is NULL\n");
        return false;
    }

    char *json_text = read_file(json_filename);
    if (!json_text)
    {
        printf("  Error: Cannot open file: %s\n", json_filename);
        return false;
    }

    cJSON *root = cJSON_Parse(json_text);
    if (!root)
    {
        printf("  Error: JSON parse error\n");
        free(json_text);
        return false;
    }

    // Lấy thông tin communication để có slave address
    uint8_t mb_slave_addr = 247; // Default
    cJSON *comm = cJSON_GetObjectItem(root, "communication");
    if (comm)
    {
        cJSON *addr = cJSON_GetObjectItem(comm, "mb_slave_addr");
        if (addr)
            mb_slave_addr = addr->valueint & 0xFF;
    }

    // Lấy register_groups
    cJSON *groups = cJSON_GetObjectItem(root, "register_groups");
    if (!groups)
    {
        printf("  Error: No 'register_groups' found in JSON\n");
        cJSON_Delete(root);
        free(json_text);
        return false;
    }

    // Cấp phát bộ nhớ cho các mảng (dynamic allocation)
    // Ước tính: mỗi nhóm ~20 phần tử, chuẩn bị sẵn
    const int MAX_PARAMS_PER_GROUP = 50;

    // Khởi tạo từng nhóm
    cJSON *init_group = cJSON_GetObjectItem(groups, "initialization");
    if (init_group)
    {
        initialization_parameters = (mb_parameter_descriptor_t *)malloc(MAX_PARAMS_PER_GROUP * sizeof(mb_parameter_descriptor_t));
        num_param_init = init_parameter_array_from_group(
            init_group, "initialization", initialization_parameters, MAX_PARAMS_PER_GROUP, mb_slave_addr);
        printf("  Initialized 'initialization' with %d parameters\n", num_param_init);
    }

    cJSON *runtime_group = cJSON_GetObjectItem(groups, "runtime_data");
    if (runtime_group)
    {
        runtime_data_parameters = (mb_parameter_descriptor_t *)malloc(MAX_PARAMS_PER_GROUP * sizeof(mb_parameter_descriptor_t));
        num_param_runtime = init_parameter_array_from_group(
            runtime_group, "runtime_data", runtime_data_parameters, MAX_PARAMS_PER_GROUP, mb_slave_addr);
        printf("  Initialized 'runtime_data' with %d parameters\n", num_param_runtime);
    }

    cJSON *battery_group = cJSON_GetObjectItem(groups, "battery_data");
    if (battery_group)
    {
        battery_data_parameters = (mb_parameter_descriptor_t *)malloc(MAX_PARAMS_PER_GROUP * sizeof(mb_parameter_descriptor_t));
        num_param_battery = init_parameter_array_from_group(
            battery_group, "battery_data", battery_data_parameters, MAX_PARAMS_PER_GROUP, mb_slave_addr);
        printf("  Initialized 'battery_data' with %d parameters\n", num_param_battery);
    }

    cJSON *accum_group = cJSON_GetObjectItem(groups, "energy_accumulation");
    if (accum_group)
    {
        energy_accumulation_parameters = (mb_parameter_descriptor_t *)malloc(MAX_PARAMS_PER_GROUP * sizeof(mb_parameter_descriptor_t));
        num_param_accumulation = init_parameter_array_from_group(
            accum_group, "energy_accumulation", energy_accumulation_parameters, MAX_PARAMS_PER_GROUP, mb_slave_addr);
        printf("  Initialized 'energy_accumulation' with %d parameters\n", num_param_accumulation);
    }

    cJSON *cons_group = cJSON_GetObjectItem(groups, "energy_consumption");
    if (cons_group)
    {
        energy_consumption_parameters = (mb_parameter_descriptor_t *)malloc(MAX_PARAMS_PER_GROUP * sizeof(mb_parameter_descriptor_t));
        num_param_comsumption = init_parameter_array_from_group(
            cons_group, "energy_consumption", energy_consumption_parameters, MAX_PARAMS_PER_GROUP, mb_slave_addr);
        printf("  Initialized 'energy_consumption' with %d parameters\n", num_param_comsumption);
    }

    cJSON *time_group = cJSON_GetObjectItem(groups, "time_data");
    if (time_group)
    {
        time_data_parameters = (mb_parameter_descriptor_t *)malloc(MAX_PARAMS_PER_GROUP * sizeof(mb_parameter_descriptor_t));
        num_param_time = init_parameter_array_from_group(
            time_group, "time_data", time_data_parameters, MAX_PARAMS_PER_GROUP, mb_slave_addr);
        printf("  Initialized 'time_data' with %d parameters\n", num_param_time);
    }

    cJSON_Delete(root);
    free(json_text);

    printf("  All parameter arrays initialized successfully from JSON\n");
    return true;
}

/**
 * Giải phóng bộ nhớ của các mảng tham số
 */
void cleanup_parameters(void)
{
    // Free strings inside each parameter, then free arrays
    if (initialization_parameters)
    {
        for (int i = 0; i < num_param_init; ++i)
        {
            if (initialization_parameters[i].param_key)
                free((void *)initialization_parameters[i].param_key);
            if (initialization_parameters[i].param_units)
                free((void *)initialization_parameters[i].param_units);
        }
        free(initialization_parameters);
        initialization_parameters = NULL;
        num_param_init = 0;
    }
    if (runtime_data_parameters)
    {
        for (int i = 0; i < num_param_runtime; ++i)
        {
            if (runtime_data_parameters[i].param_key)
                free((void *)runtime_data_parameters[i].param_key);
            if (runtime_data_parameters[i].param_units)
                free((void *)runtime_data_parameters[i].param_units);
        }
        free(runtime_data_parameters);
        runtime_data_parameters = NULL;
        num_param_runtime = 0;
    }
    if (battery_data_parameters)
    {
        for (int i = 0; i < num_param_battery; ++i)
        {
            if (battery_data_parameters[i].param_key)
                free((void *)battery_data_parameters[i].param_key);
            if (battery_data_parameters[i].param_units)
                free((void *)battery_data_parameters[i].param_units);
        }
        free(battery_data_parameters);
        battery_data_parameters = NULL;
        num_param_battery = 0;
    }
    if (energy_accumulation_parameters)
    {
        for (int i = 0; i < num_param_accumulation; ++i)
        {
            if (energy_accumulation_parameters[i].param_key)
                free((void *)energy_accumulation_parameters[i].param_key);
            if (energy_accumulation_parameters[i].param_units)
                free((void *)energy_accumulation_parameters[i].param_units);
        }
        free(energy_accumulation_parameters);
        energy_accumulation_parameters = NULL;
        num_param_accumulation = 0;
    }
    if (energy_consumption_parameters)
    {
        for (int i = 0; i < num_param_comsumption; ++i)
        {
            if (energy_consumption_parameters[i].param_key)
                free((void *)energy_consumption_parameters[i].param_key);
            if (energy_consumption_parameters[i].param_units)
                free((void *)energy_consumption_parameters[i].param_units);
        }
        free(energy_consumption_parameters);
        energy_consumption_parameters = NULL;
        num_param_comsumption = 0;
    }
    if (time_data_parameters)
    {
        for (int i = 0; i < num_param_time; ++i)
        {
            if (time_data_parameters[i].param_key)
                free((void *)time_data_parameters[i].param_key);
            if (time_data_parameters[i].param_units)
                free((void *)time_data_parameters[i].param_units);
        }
        free(time_data_parameters);
        time_data_parameters = NULL;
        num_param_time = 0;
    }
}
