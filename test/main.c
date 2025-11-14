#include <stdio.h>
#include <stdint.h>
#include "json_parser.h"
#include "error_handler.h"

#include <stdio.h>
#include "device_manager.h"

// Chuyển đổi enum sang chuỗi để in (tối giản)
static const char* mb_param_type_to_str(mb_param_type_t t) {
    switch(t) {
        case MB_PARAM_HOLDING: return "HOLDING";
        case MB_PARAM_INPUT: return "INPUT";
        case MB_PARAM_COIL: return "COIL";
        case MB_PARAM_DISCRETE: return "DISCRETE";
        default: return "UNKNOWN";
    }
}
static const char* mb_descr_type_to_str(mb_descr_type_t t) {
    switch(t) {
        case PARAM_TYPE_U8: return "u8";
        case PARAM_TYPE_U16: return "u16";
        case PARAM_TYPE_U32: return "u32";
        case PARAM_TYPE_FLOAT: return "float";
        case PARAM_TYPE_ASCII: return "ascii";
        default: return "other";
    }
}
static const char* perms_to_str(mb_param_perms_t p) {
    if (p == PAR_PERMS_READ) return "ro";
    if (p == PAR_PERMS_WRITE) return "wo";
    if (p == PAR_PERMS_READ_WRITE) return "rw";
    return "other";
}

void print_param(const mb_parameter_descriptor_t *p, int index) {
    if (!p) {
        printf("Parameter %d is NULL\n", index);
        return;
    }
    printf("Parameter[%d]:\n", index);
    printf("  cid: %u\n", (unsigned)p->cid);
    printf("  param_key: %s\n", p->param_key != NULL ? p->param_key : "(null)");
    printf("  param_units: %s\n", p->param_units ? p->param_units : "(null)");
    printf("  mb_slave_addr: %u\n", p->mb_slave_addr);
    printf("  mb_param_type: %s\n", mb_param_type_to_str(p->mb_param_type));
    printf("  mb_reg_start: %u\n", p->mb_reg_start);
    printf("  mb_size (registers): %u\n", p->mb_size);
    printf("  param_offset: %u\n", p->param_offset);
    printf("  param_type: %s\n", mb_descr_type_to_str(p->param_type));
    printf("  param_size (bytes): %u\n", p->param_size);
    printf("  param_opts: min=%d, max=%d, step=%d\n",
           p->param_opts.min, p->param_opts.max, p->param_opts.step);
    printf("  access: %s\n", perms_to_str(p->access));
}
int main(void) {
    // InverterConfig cfg;
    // if (!load_inverter_config("config/goodwe_GW20K-ET-L-G10_v1_0_0.json", &cfg)) {
    //     printf("Failed to load config\n");
    //     return -1;
    // }

    // printf("Connected to inverter @ addr %d, baud %d\n",
    //        cfg.comm.slave_addr, cfg.comm.baudrate);

    // // Giả lập giá trị thanh ghi lỗi đọc được từ inverter
    // uint32_t error_reg = (1UL << 3) | (1UL << 13) | (1UL << 28);
    // printf("Error Register Raw = 0x%08X\n", error_reg);

    // print_active_errors(error_reg, "config/goodwe_GW20K-ET-L-G10_v1_0_0.json");
    // return 0;
    const char *path = "config/goodwe_GW20K-ET-L-G10_v1_0_0.json";
    if (!init_all_parameters_from_json(path)) {
        fprintf(stderr, "Failed to init parameters from JSON\n");
        return 1;
    }

    if (num_param_init > 0 && initialization_parameters) {
        // Print first element (index 0) and then list all for debugging
        print_param(&initialization_parameters[0], 0);

        printf("\n--- Debug: dump all initialization parameters ---\n");
        for (int i = 0; i < num_param_init; ++i) {
            mb_parameter_descriptor_t *pp = &initialization_parameters[i];
            printf("idx=%d addr=%p cid=%u key=%p key_len=%zu units=%p\n",
                   i, (void*)pp, (unsigned)pp->cid,
                   (void*)pp->param_key, pp->param_key ? strlen(pp->param_key) : 0,
                   (void*)pp->param_units);
        }
    } else {
        printf("No initialization parameters found (num_param_init=%d)\n", num_param_init);
    }

    cleanup_parameters(); // giải phóng bộ nhớ nếu cần
    return 0;
}
