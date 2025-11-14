#include <stdio.h>
#include <stdlib.h>
#include "json_parser.h"
#include "device_manager.h"

int main(int argc, char *argv[]) {
    const char *json_file = "../profiles/goodwe_GW20K-ET-L-G10_v1_0_0.json";
    
    printf("=== Testing JSON Parser for GW20K ===\n");
    printf("Loading JSON from: %s\n\n", json_file);

    // Khởi tạo tất cả các mảng từ JSON
    if (!init_all_parameters_from_json(json_file)) {
        printf("Failed to initialize parameters from JSON\n");
        return 1;
    }

    printf("\n=== Parameter Summary ===\n");
    printf("Initialization params: %d\n", num_param_init);
    printf("Runtime data params: %d\n", num_param_runtime);
    printf("Battery data params: %d\n", num_param_battery);
    printf("Energy accumulation params: %d\n", num_param_accumulation);
    printf("Energy consumption params: %d\n", num_param_comsumption);
    printf("Time data params: %d\n", num_param_time);

    // Hiển thị một vài sample
    if (num_param_init > 0) {
        printf("\n=== Sample: First Initialization Parameter ===\n");
        printf("CID: %u\n", initialization_parameters[0].cid);
        printf("Key: %s\n", initialization_parameters[0].param_key ? initialization_parameters[0].param_key : "NULL");
        printf("Units: %s\n", initialization_parameters[0].param_units ? initialization_parameters[0].param_units : "NULL");
        printf("MB Register Start: %u\n", initialization_parameters[0].mb_reg_start);
        printf("MB Size: %u\n", initialization_parameters[0].mb_size);
        printf("Param Type: %d\n", initialization_parameters[0].param_type);
    }

    if (num_param_runtime > 0) {
        printf("\n=== Sample: First Runtime Parameter ===\n");
        printf("CID: %u\n", runtime_data_parameters[0].cid);
        printf("Key: %s\n", runtime_data_parameters[0].param_key ? runtime_data_parameters[0].param_key : "NULL");
        printf("Units: %s\n", runtime_data_parameters[0].param_units ? runtime_data_parameters[0].param_units : "NULL");
        printf("MB Register Start: %u\n", runtime_data_parameters[0].mb_reg_start);
    }

    // Cleanup
    cleanup_parameters();
    printf("\n=== Cleanup Complete ===\n");

    return 0;
}
