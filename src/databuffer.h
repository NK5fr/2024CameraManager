
// Written by: Lars Aksel Tveråmo
// 3D Motion Technologies AS

#ifndef DATA_BUFFER_H
#define DATA_BUFFER_H

#include <stdio.h>
#include <string.h>

class DataBuffer {
public:
    DataBuffer();
    ~DataBuffer();

    void createBuffer(unsigned int bufferSize);
    void copyIntoBuffer(unsigned char* buffer, unsigned int bufferSize);
    void zeroBuffer();
    void releaseBuffer();

    inline unsigned int getUsedBuffer() const { return used; }
    inline unsigned int getCapacity() const { return capacity; }
    inline unsigned char* getDataBuffer() const { return dataBuffer; }

private:
    unsigned char* dataBuffer = nullptr;
    unsigned int capacity = 0;  // Capacity of buffer can be bigger than actually used...
    unsigned int used = 0;
};

#endif