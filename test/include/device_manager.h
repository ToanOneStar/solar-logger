#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef enum
{
    MB_PARAM_HOLDING = 0x00, /*!< Modbus Holding register. */
    MB_PARAM_INPUT,          /*!< Modbus Input register. */
    MB_PARAM_COIL,           /*!< Modbus Coils. */
    MB_PARAM_DISCRETE,       /*!< Modbus Discrete bits. */
    MB_PARAM_COUNT,
    MB_PARAM_UNKNOWN = 0xFF
} mb_param_type_t;

typedef enum
{
    PARAM_TYPE_U8 = 0x00,              /*!< Unsigned 8 */
    PARAM_TYPE_U16 = 0x01,             /*!< Unsigned 16 */
    PARAM_TYPE_U32 = 0x02,             /*!< Unsigned 32 */
    PARAM_TYPE_FLOAT = 0x03,           /*!< Float type */
    PARAM_TYPE_ASCII = 0x04,           /*!< ASCII type */
    PARAM_TYPE_BIN = 0x07,             /*!< BIN type */
    PARAM_TYPE_I8_A = 0x0A,            /*!< I8 signed integer in high byte of register */
    PARAM_TYPE_I8_B = 0x0B,            /*!< I8 signed integer in low byte of register */
    PARAM_TYPE_U8_A = 0x0C,            /*!< U8 unsigned integer written to hi byte of register */
    PARAM_TYPE_U8_B = 0x0D,            /*!< U8 unsigned integer written to low byte of register */
    PARAM_TYPE_I16_AB = 0x0E,          /*!< I16 signed integer, big endian */
    PARAM_TYPE_I16_BA = 0x0F,          /*!< I16 signed integer, little endian */
    PARAM_TYPE_U16_AB = 0x10,          /*!< U16 unsigned integer, big endian*/
    PARAM_TYPE_U16_BA = 0x11,          /*!< U16 unsigned integer, little endian */
    PARAM_TYPE_I32_ABCD = 0x12,        /*!< I32 ABCD signed integer, big endian */
    PARAM_TYPE_I32_CDAB = 0x13,        /*!< I32 CDAB signed integer, big endian, reversed register order */
    PARAM_TYPE_I32_BADC = 0x14,        /*!< I32 BADC signed integer, little endian, reversed register order */
    PARAM_TYPE_I32_DCBA = 0x15,        /*!< I32 DCBA signed integer, little endian */
    PARAM_TYPE_U32_ABCD = 0x16,        /*!< U32 ABCD unsigned integer, big endian */
    PARAM_TYPE_U32_CDAB = 0x17,        /*!< U32 CDAB unsigned integer, big endian, reversed register order */
    PARAM_TYPE_U32_BADC = 0x18,        /*!< U32 BADC unsigned integer, little endian, reversed register order */
    PARAM_TYPE_U32_DCBA = 0x19,        /*!< U32 DCBA unsigned integer, little endian */
    PARAM_TYPE_FLOAT_ABCD = 0x1A,      /*!< Float ABCD floating point, big endian */
    PARAM_TYPE_FLOAT_CDAB = 0x1B,      /*!< Float CDAB floating point big endian, reversed register order */
    PARAM_TYPE_FLOAT_BADC = 0x1C,      /*!< Float BADC floating point, little endian, reversed register order */
    PARAM_TYPE_FLOAT_DCBA = 0x1D,      /*!< Float DCBA floating point, little endian */
    PARAM_TYPE_I64_ABCDEFGH = 0x1E,    /*!< I64, ABCDEFGH signed integer, big endian */
    PARAM_TYPE_I64_HGFEDCBA = 0x1F,    /*!< I64, HGFEDCBA signed integer, little endian */
    PARAM_TYPE_I64_GHEFCDAB = 0x20,    /*!< I64, GHEFCDAB signed integer, big endian, reversed register order */
    PARAM_TYPE_I64_BADCFEHG = 0x21,    /*!< I64, BADCFEHG signed integer, little endian, reversed register order */
    PARAM_TYPE_U64_ABCDEFGH = 0x22,    /*!< U64, ABCDEFGH unsigned integer, big endian */
    PARAM_TYPE_U64_HGFEDCBA = 0x23,    /*!< U64, HGFEDCBA unsigned integer, little endian */
    PARAM_TYPE_U64_GHEFCDAB = 0x24,    /*!< U64, GHEFCDAB unsigned integer, big endian, reversed register order */
    PARAM_TYPE_U64_BADCFEHG = 0x25,    /*!< U64, BADCFEHG unsigned integer, little endian, reversed register order */
    PARAM_TYPE_DOUBLE_ABCDEFGH = 0x26, /*!< Double ABCDEFGH floating point, big endian*/
    PARAM_TYPE_DOUBLE_HGFEDCBA = 0x27, /*!< Double HGFEDCBA floating point, little endian*/
    PARAM_TYPE_DOUBLE_GHEFCDAB = 0x28, /*!< Double GHEFCDAB floating point, big endian, reversed register order */
    PARAM_TYPE_DOUBLE_BADCFEHG = 0x29  /*!< Double BADCFEHG floating point, little endian, reversed register order */
} mb_descr_type_t;

typedef enum {
    PARAM_SIZE_U8 = 0x01,                   /*!< Unsigned 8 */
    PARAM_SIZE_U8_REG = 0x02,               /*!< Unsigned 8, register value */
    PARAM_SIZE_I8_REG = 0x02,               /*!< Signed 8, register value */
    PARAM_SIZE_I16 = 0x02,                  /*!< Unsigned 16 */
    PARAM_SIZE_U16 = 0x02,                  /*!< Unsigned 16 */
    PARAM_SIZE_I32 = 0x04,                  /*!< Signed 32 */
    PARAM_SIZE_U32 = 0x04,                  /*!< Unsigned 32 */
    PARAM_SIZE_FLOAT = 0x04,                /*!< Float 32 size */
    PARAM_SIZE_ASCII = 0x08,                /*!< ASCII size default*/
    PARAM_SIZE_ASCII24 = 0x18,              /*!< ASCII24 size */
    PARAM_SIZE_I64 = 0x08,                  /*!< Signed integer 64 size */
    PARAM_SIZE_U64 = 0x08,                  /*!< Unsigned integer 64 size */
    PARAM_SIZE_DOUBLE = 0x08,               /*!< Double 64 size */
    PARAM_MAX_SIZE
} mb_descr_size_t;

typedef union {
    struct {
        int opt1;                         /*!< Parameter option1 */
        int opt2;                         /*!< Parameter option2 */
        int opt3;                         /*!< Parameter option3 */
    }; /*!< Parameter options version 1 */
    struct {
        int min;                          /*!< Parameter minimum value */
        int max;                          /*!< Parameter maximum value */
        int step;                         /*!< Step of parameter change tracking */
    }; /*!< Parameter options version 2 */
} mb_parameter_opt_t;

typedef enum {
    PAR_PERMS_READ               = 0,                                        /**< the characteristic of the device are readable */
    PAR_PERMS_WRITE              = 1,                                        /**< the characteristic of the device are writable*/
    PAR_PERMS_TRIGGER            = 2,                                        /**< the characteristic of the device are triggerable */
    PAR_PERMS_READ_WRITE         = PAR_PERMS_READ | PAR_PERMS_WRITE,            /**< the characteristic of the device are readable & writable */
    PAR_PERMS_READ_TRIGGER       = PAR_PERMS_READ | PAR_PERMS_TRIGGER,          /**< the characteristic of the device are readable & triggerable */
    PAR_PERMS_WRITE_TRIGGER      = PAR_PERMS_WRITE | PAR_PERMS_TRIGGER,         /**< the characteristic of the device are writable & triggerable */
    PAR_PERMS_READ_WRITE_TRIGGER = PAR_PERMS_READ_WRITE | PAR_PERMS_TRIGGER,    /**< the characteristic of the device are readable & writable & triggerable */
} mb_param_perms_t;

typedef struct
{
    uint16_t cid;                  /*!< Characteristic cid */
    const char *param_key;         /*!< The key (name) of the parameter */
    const char *param_units;       /*!< The physical units of the parameter */
    uint8_t mb_slave_addr;         /*!< Slave address of device in the Modbus segment */
    mb_param_type_t mb_param_type; /*!< Type of modbus parameter */
    uint16_t mb_reg_start;         /*!< This is the Modbus register address. This is the 0 based value. */
    uint16_t mb_size;              /*!< Size of mb parameter in registers */
    uint16_t param_offset;         /*!< Parameter name (OFFSET in the parameter structure) */
    mb_descr_type_t param_type;    /*!< Float, U8, U16, U32, ASCII, etc. */
    mb_descr_size_t param_size;    /*!< Number of bytes in the parameter. */
    mb_parameter_opt_t param_opts; /*!< Parameter options used to check limits and etc. */
    mb_param_perms_t access;       /*!< Access permissions based on mode */
} mb_parameter_descriptor_t;

// Table to storage initialization register
extern int num_param_init;
extern mb_parameter_descriptor_t *initialization_parameters;
// Table to storage runtime data register
extern int num_param_runtime;
extern mb_parameter_descriptor_t *runtime_data_parameters;
// Table to storage battery data register
extern int num_param_battery;
extern mb_parameter_descriptor_t *battery_data_parameters;
// Table to storage energy accumulation register
extern int num_param_accumulation;
extern mb_parameter_descriptor_t *energy_accumulation_parameters;
// Table to storage energy consumption register
extern int num_param_comsumption;
extern mb_parameter_descriptor_t *energy_consumption_parameters;
// Table to storage time data register
extern int num_param_time;
extern mb_parameter_descriptor_t *time_data_parameters;

#endif