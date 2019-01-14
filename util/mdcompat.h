#ifndef MY_I2C_H_INCLUDED
#define MY_I2C_H_INCLUDED

/**
 * Compatiblity Declarations
 */
#define PI 3.1415926536
unsigned short constrain(unsigned short x,unsigned short a, unsigned short b);

#ifdef __cplusplus
extern "C" {
#endif
 
int mbed_i2c_write(
    unsigned char slave_addr,
    unsigned char reg_addr,
    unsigned char length,
    unsigned char *data);
 
int mbed_i2c_read(
    unsigned char slave_addr,
    unsigned char reg_addr,
    unsigned char length,
    unsigned char *data);
 
int delay_ms(
    unsigned long num_ms);
 
void get_ms(
    unsigned long *count);
 
int reg_int_cb(
    void (*cb)(void),
    unsigned char port,
    unsigned char pin);
 
long labs(long x);
 
float fabsf(float x);
 
int min(int a, int b);
 
 
#ifdef __cplusplus
}
#endif
 
#endif // MY_I2C_H_INCLUDED