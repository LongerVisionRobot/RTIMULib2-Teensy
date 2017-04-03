////////////////////////////////////////////////////////////////////////////
//
//  This file is part of RTIMULib
//
//  Copyright (c) 2014-2015, richards-tech, LLC
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy of
//  this software and associated documentation files (the "Software"), to deal in
//  the Software without restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
//  Software, and to permit persons to whom the Software is furnished to do so,
//  subject to the following conditions:
//
//  The above copyright notice and this permission notice shall be included in all
//  copies or substantial portions of the Software.
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
//  INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
//  PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
//  OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

//  The MPU-9255 and SPI driver code is based on code generously supplied by
//  staslock@gmail.com (www.clickdrive.io)


#ifndef _RTIMUMPU9255_H
#define	_RTIMUMPU9255_H

#include "RTIMU.h"

//  Define this symbol to use cache mode

#define MPU9255_CACHE_MODE
#define MPU9255_FIFO_WITH_TEMP    1
#define MPU9255_FIFO_WITH_COMPASS 1
//  FIFO transfer size

#if MPU9255_FIFO_WITH_TEMP == 1
    #if MPU9255_FIFO_WITH_COMPASS == 1
        #define MPU9255_FIFO_CHUNK_SIZE 22           // adding compass/slave1 adds 6 bytes plus 2 bytes for status for AKA compass 
    #else 
        #define MPU9255_FIFO_CHUNK_SIZE 14           // adding temperature adds 2 bytes
    #endif
#else
    #if MPU9255_FIFO_WITH_COMPASS == 1
        #define MPU9255_FIFO_CHUNK_SIZE 20          // adding compass 8 bytes max
    #else 
        #define MPU9255_FIFO_CHUNK_SIZE 12          // gyro and accels take 12 bytes
    #endif
#endif
#ifdef MPU9255_CACHE_MODE
//  Cache mode defines
#define MPU9255_CACHE_SIZE          16                      // number of chunks in a block
#define MPU9255_CACHE_BLOCK_COUNT   16                      // number of cache blocks

typedef struct
{
    unsigned char data[MPU9255_FIFO_CHUNK_SIZE * MPU9255_CACHE_SIZE];
    int count;                                              // number of chunks in the cache block
    int index;                                              // current index into the cache
    // if temperature and compass are not read through FIFO
    #if MPU9255_FIFO_WITH_COMPASS == 0
    unsigned char compass[8];                               // the raw compass readings for the block
    #endif
    #if MPU9255_FIFO_WITH_TEMP == 0
    unsigned char temperature[2];                           // the raw temperature reading
    #endif
} MPU9255_CACHE_BLOCK;

#endif

#define TEMPERATURE_DELTA 0.05f                             // change in temperature necessary to recompute the biases
class RTIMUMPU9255 : public RTIMU
{
public:
    RTIMUMPU9255(RTIMUSettings *settings);
    ~RTIMUMPU9255();
	
    bool setGyroLpf(unsigned char lpf);
    bool setAccelLpf(unsigned char lpf);
    bool setSampleRate(int rate);
    bool setCompassRate(int rate);
    bool setGyroFsr(unsigned char fsr);
    bool setAccelFsr(unsigned char fsr);

    virtual const char *IMUName() { return "MPU-9255"; }
    virtual int IMUType() { return RTIMU_TYPE_MPU9255; }
    virtual bool IMUInit();
    virtual bool IMURead();
    virtual int IMUGetPollInterval();

protected:

    RTFLOAT m_compassAdjust[3];                             // the compass fuse ROM values converted for use

private:
    bool setGyroConfig();
    bool setAccelConfig();
    bool setSampleRate();
    bool compassSetup();
    bool setCompassRate();
    bool resetFifo();
    bool bypassOn();
    bool bypassOff();

    bool m_firstTime;                                       // if first sample

    RTFLOAT m_temperature_previous;
    unsigned char m_slaveAddr;                              // I2C address of MPU9150

    unsigned char m_gyroLpf;                                // gyro low pass filter setting
    unsigned char m_accelLpf;                               // accel low pass filter setting
    int m_compassRate;                                      // compass sample rate in Hz
    unsigned char m_gyroFsr;
    unsigned char m_accelFsr;

    RTFLOAT m_gyroScale;
    RTFLOAT m_accelScale;


#ifdef MPU9255_CACHE_MODE
    MPU9255_CACHE_BLOCK m_cache[MPU9255_CACHE_BLOCK_COUNT]; // the cache itself
    int m_cacheIn;                                          // the in index
    int m_cacheOut;                                         // the out index
    int m_cacheCount;                                       // number of used cache blocks
#endif

};

#endif // _RTIMUMPU9255_H
