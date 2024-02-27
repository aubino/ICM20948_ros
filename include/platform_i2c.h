#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdint.h>
#include <stddef.h>
#include <math.h>
#include "../icm20948/inc/icm20948_api.h"
#ifndef __PLATFORMI2C_H__
#define __PLATFORMI2C_H__
#define DEVICE "/dev/i2c-1" // Adjust this path according to your system


int file  =  open(DEVICE, O_RDWR);   
// Open the I2C bus
if(file  < 0) {
    printf("Failed to open the bus.");
    exit(EXIT_FAILURE);
    }

int8_t usr_write(const uint8_t addr, const uint8_t *data, const uint32_t len) {
    // Prepare the I2C message
    struct i2c_msg messages[1];
    messages[0].addr = addr;
    messages[0].flags = 0;
    messages[0].len = len ;
    messages[0].buf = (unsigned char *)&data;
    // Prepare the I2C transfer
    struct i2c_rdwr_ioctl_data transfer;
    transfer.msgs = messages;
    transfer.nmsgs = 1;
    // Assert the CS
     if (ioctl(file, I2C_RDWR, &transfer) < 0)
    {
        printf("Failed to acquire bus access and/or talk to slave.\n");
        return ICM20948_RET_GEN_FAIL ;
    }
    return ICM20948_RET_OK;
}

int8_t usr_read(const uint8_t addr, uint8_t *data, const uint32_t len) {
    icm20948_return_code_t ret = ICM20948_RET_OK;
    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        perror("Failed to acquire bus access and/or talk to slave.");
        return -1;
    }

    // Prepare the I2C message for reading
    struct i2c_msg message;
    message.addr = addr;
    message.flags = I2C_M_RD; // Read operation flag
    message.len = len;
    message.buf = data;

    // Prepare the I2C transfer
    struct i2c_rdwr_ioctl_data transfer;
    transfer.msgs = &message;
    transfer.nmsgs = 1; // Number of messages

    // Perform the I2C read operation
    if (ioctl(file, I2C_RDWR, &transfer) < 0) {
        perror("Failed to read from the I2C device.");
        return -1;
    }
    return ret;
}

void usr_delay_us(uint32_t period) {
    // Delay for the requested period
}
#endif //__USERI2C_H__