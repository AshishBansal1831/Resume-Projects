#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdint.h>

typedef enum 
{
    TEMP_SAMPLING = 10,
    ADC_SAMPLING = 20
}Config_Type;

typedef struct{
    char string[80];
    size_t length;
}ConfigSendQueue;

typedef struct
{
    Config_Type Type;
    uint32_t Value;
}Configurations;

typedef enum QOS
{
    QOS0,
    QOS1,
    QOS2,
}QOS;

#endif /* _COMMON_H_ */