
#include "databuffer.h"

DataBuffer::DataBuffer() {
}

DataBuffer::~DataBuffer() {
    delete[] dataBuffer;
}

void DataBuffer::createBuffer(unsigned int bufferSize) {
    if (capacity < bufferSize) {
        if (dataBuffer != nullptr) delete[] dataBuffer;
        dataBuffer = new unsigned char[bufferSize];
        this->capacity = bufferSize;
    }
    this->used = bufferSize;
}

void DataBuffer::copyIntoBuffer(unsigned char* buffer, unsigned int bufferSize) {
    if (capacity < bufferSize) createBuffer(bufferSize);
    memcpy(dataBuffer, buffer, bufferSize);
    this->used = bufferSize;
}

void DataBuffer::zeroBuffer() {
    memset(dataBuffer, 0, capacity);
}

void DataBuffer::releaseBuffer() {
    delete[] dataBuffer;
    used = 0;
    capacity = 0;
}
