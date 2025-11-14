#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

/* Điền mô tả thực tế cho từng bit theo tài liệu (Error Table 8-2) */
static const char *error_bit_desc[32] = {
    "Bit0: Description for bit 0",
    "Bit1: Description for bit 1",
    "Bit30: Description for bit 30",
    "Bit31: Description for bit 31",
    "Bit0: Description for bit 0",
    "Bit1: Description for bit 1",
    "Bit30: Description for bit 30",
    "Bit31: Description for bit 31",
    "Bit0: Description for bit 0",
    "Bit1: Description for bit 1",
    "Bit30: Description for bit 30",
    "Bit31: Description for bit 31",
    "Bit0: Description for bit 0",
    "Bit1: Description for bit 1",
    "Bit30: Description for bit 30",
    "Bit31: Description for bit 31",
    "Bit0: Description for bit 0",
    "Bit1: Description for bit 1",
    "Bit30: Description for bit 30",
    "Bit31: Description for bit 31",
    "Bit0: Description for bit 0",
    "Bit1: Description for bit 1",
    "Bit30: Description for bit 30",
    "Bit31: Description for bit 31",
    "Bit0: Description for bit 0",
    "Bit1: Description for bit 1",
    "Bit30: Description for bit 30",
    "Bit31: Description for bit 31",
    "Bit0: Description for bit 0",
    "Bit1: Description for bit 1",
    "Bit30: Description for bit 30",
    "Bit31: Description for bit 31",
};

void decode_and_log_errors(uint32_t errbits)
{
    int any = 0;
    for (int i = 0; i < 32; ++i) {
        if (errbits & (1u << i)) {
            printf("Error bit %d set: %s\n", i, error_bit_desc[i] ? error_bit_desc[i] : "(no desc)");
            any = 1;
        }
    }
    if (!any) {
        printf("No error bits set (0x%08X)", errbits);
    }
}

void read_error_message_example(void)
{
    uint32_t errbits = 0xFFFFFFFF;

    // errbits now holds the 32-bit mask, decode it
    decode_and_log_errors(errbits);
}

int main(){
    read_error_message_example();
}