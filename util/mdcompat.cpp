#include "mdcompat.h"
#include <mbed.h>
#define BUFFER_SIZE 128
// must be out of the scope of extern "C"

I2C imu_i2c(MPU9250_I2C_SDA, MPU9250_I2C_SCL);
Timer imu_timer;

volatile uint8_t i2c_buffer[BUFFER_SIZE];
volatile bool i2c_busy = false;

unsigned short constrain(
    unsigned short x,
    unsigned short a,
    unsigned short b)
{
    unsigned short result;
    result = (x < a) ? a : x;
    result = (x > b) ? b : x;
    return result;
}

#if defined(MPU9250_I2C_RTOS_IMPL) && MPU9250_I2C_RTOS_IMPL > 0

int delay_ms(
    unsigned long num_ms)
{
    ThisThread::sleep_for(num_ms);
    return 0;
}

static void i2c_event_handler_cb(int res)
{
    i2c_busy = false;
}

int mbed_i2c_read(
    unsigned char slave_addr,
    unsigned char reg_addr,
    unsigned char length,
    unsigned char *data)
{
    i2c_busy = true;
    // const char RA[] = {reg_addr};
    int status = imu_i2c.transfer((int)slave_addr << 1,(char*)&reg_addr,1,(char *)data,length,i2c_event_handler_cb); 
    if(status!=0)
    {
        return 1;
    }
    while(i2c_busy){ThisThread::yield();} //FIXME better impl
    return 0;
}

int mbed_i2c_write(
    unsigned char slave_addr,
    unsigned char reg_addr,
    unsigned char length,
    unsigned char *data) {
    i2c_buffer[0] = reg_addr;
    memcpy((void *)i2c_buffer+1,data,length);
    i2c_busy = true;
    int status = imu_i2c.transfer((int)slave_addr << 1,(char*)&i2c_buffer,length+1,NULL,0,i2c_event_handler_cb); 
    if(status!=0)
    {
        return 1;
    }
    while(imu_i2c.){ThisThread::yield();}//FIXME better impl
    return 0;
}

#else

int delay_ms(
    unsigned long num_ms)
{
    wait_ms(num_ms);
    return 0;
}

int mbed_i2c_read(
    unsigned char slave_addr,
    unsigned char reg_addr,
    unsigned char length,
    unsigned char *data)
{
    int result = 0;
    const char RA[] = {reg_addr};
    result += imu_i2c.write((int)slave_addr << 1, RA, 1, 1);
    result += imu_i2c.read((int)slave_addr << 1, (char *)data, length, 0);
    //TODO: change to nonblocking using transfer()
    return result;
}

int mbed_i2c_write(
    unsigned char slave_addr,
    unsigned char reg_addr,
    unsigned char length,
    unsigned char *data) {
    i2c_buffer[0] = reg_addr;
    memcpy((void *)i2c_buffer+1,data,length);
    int result = imu_i2c.write((int)slave_addr << 1, (char*)i2c_buffer, length+1, 0);
    //TODO: change to nonblocking using transfer()
    return result;
}

#endif /*MBED_CONF_RTOS_PRESENT*/


void get_ms(unsigned long *count)
{
    *count=imu_timer.read_ms();
}

int reg_int_cb(
    void (*cb)(void),
    unsigned char port,
    unsigned char pin)
{
    return 0;
}

long labs(long x)
{
    return x > 0 ? x : -x;
}

float fabsf(float x)
{
    return x > 0 ? x : -x;
}

int min(int a, int b)
{
    return a > b ? b : a;
}