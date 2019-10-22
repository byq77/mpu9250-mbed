#include "mdcompat.h"
#include <mbed.h>
#define BUFFER_SIZE 128
// must be out of the scope of extern "C"

#if defined(MPU9250_I2C_FREQUENCY)
    #define TIMEOUT_NON_BLOCKING_BYTE ((SystemCoreClock / MPU9250_I2C_FREQUENCY) * 3 * 10)
#else
    #define TIMEOUT_NON_BLOCKING_BYTE ((SystemCoreClock / 100000U) * 3 * 10)
#endif

I2C * imu_i2c = NULL;
Timer * imu_timer = NULL;

volatile uint8_t i2c_buffer[BUFFER_SIZE];
volatile uint8_t transmission_status;

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

static void i2c_event_handler_cb(int event)
{
   /*
    *#define I2C_EVENT_ERROR               (1 << 1)
    *#define I2C_EVENT_ERROR_NO_SLAVE      (1 << 2)
    *#define I2C_EVENT_TRANSFER_COMPLETE   (1 << 3)
    *#define I2C_EVENT_TRANSFER_EARLY_NACK (1 << 4)
    *#define I2C_EVENT_ALL                 (I2C_EVENT_ERROR |  I2C_EVENT_TRANSFER_COMPLETE | I2C_EVENT_ERROR_NO_SLAVE | I2C_EVENT_TRANSFER_EARLY_NACK)
    */
    transmission_status = (uint8_t)event;
}

int mbed_i2c_read(
    unsigned char slave_addr,
    unsigned char reg_addr,
    unsigned char length,
    unsigned char *data)
{
    int guard = TIMEOUT_NON_BLOCKING_BYTE * (length + 3); //TODO: find better method for timeout
    transmission_status = 0xff; // set flag
    
    if(imu_i2c->transfer((int)slave_addr << 1,(const char*)&reg_addr,1,(char *)data,length,i2c_event_handler_cb)!=0)
    {
        // busy
        return 1;
    }
    while(transmission_status == 0xff && guard--)
    // while(transmission_status == 0xff)
    {
        ThisThread::yield();
        if(guard==0)
        {
            // debug_print("transferInternal: TIMEOUT!\r\n");
            imu_i2c->abort_transfer();
            return 1;
        }
    }
    // decode the event
    return (transmission_status = I2C_EVENT_TRANSFER_COMPLETE & transmission_status ? 0 : 1);
}

int mbed_i2c_write(
    unsigned char slave_addr,
    unsigned char reg_addr,
    unsigned char length,
    unsigned char *data) {
    int guard = TIMEOUT_NON_BLOCKING_BYTE * (length + 3); //TODO: find better method for timeout
    transmission_status = 0xff; // set flag
    i2c_buffer[0] = reg_addr;
    memcpy((void *)i2c_buffer+1,data,length);
    if(imu_i2c->transfer((int)slave_addr << 1,(char*)&i2c_buffer,length+1,NULL,0,i2c_event_handler_cb)!=0) 
    {
        // busy
        return 1;
    }
    while(transmission_status == 0xff && guard--)
    // while(transmission_status == 0xff)
    {
        ThisThread::yield();
        if(guard==0)
        {
            // debug_print("transferInternal: TIMEOUT!\r\n");
            imu_i2c->abort_transfer();
            return 1;
        }
    }
    // decode the event
    return (transmission_status = I2C_EVENT_TRANSFER_COMPLETE & transmission_status ? 0 : 1);
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
    result += imu_i2c->write((int)slave_addr << 1, RA, 1, 1);
    result += imu_i2c->read((int)slave_addr << 1, (char *)data, length, 0);
    return result;
    // return 0;
}

int mbed_i2c_write(
    unsigned char slave_addr,
    unsigned char reg_addr,
    unsigned char length,
    unsigned char *data) {
    i2c_buffer[0] = reg_addr;
    memcpy((void *)i2c_buffer+1,data,length);
    int result = imu_i2c->write((int)slave_addr << 1, (char*)i2c_buffer, length+1, 0);
    return result;
    // return 0;
}

#endif /*MBED_CONF_RTOS_PRESENT*/


void get_ms(unsigned long *count)
{
    *count=imu_timer->read_ms();
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