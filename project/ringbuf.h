/*------------------------------------------------------------------------------
 * @file ringbuf.h
 * @brief Lock-free single-producer single-consumer ringbuffer
 *
 * @resources
 * verbatim from:
 * https://landenlabs.com/code/ring/ring.html
 *----------------------------------------------------------------------------*/

#ifndef RINGBUF_H
#define RINGBUF_H

//
//  Simple fixed size ring buffer.
//  Manage objects by value.
//  Thread safe for single Producer and single Consumer.
//

#pragma once

template <class T> //, size_t RingSize>
class RingBuffer
{
public:
    RingBuffer(size_t size = 100) 
        : m_size(size), m_buffer(new T[size]), m_rIndex(0), m_wIndex(0) 
        { assert(size > 1 && m_buffer != NULL); }

    ~RingBuffer() 
        { delete [] m_buffer; };

    size_t Next(size_t n) const 
        { return (n+1)%m_size; }
    bool Empty() const 
        { return (m_rIndex == m_wIndex); }
    bool Full() const
        { return (Next(m_wIndex) == m_rIndex); }

    bool Put(const T& value)
    {
        if (Full()) 
            return false;
        m_buffer[m_wIndex] = value;
        m_wIndex = Next(m_wIndex);
        return true;
    }

    bool Get(T& value)
    {
        if (Empty())
            return false;
        value = m_buffer[m_rIndex];
        m_rIndex = Next(m_rIndex);
        return true;
    }

private:
    size_t          m_size;
    T*              m_buffer;

    // volatile is only used to keep compiler from placing values in registers.
    // volatile does NOT make the index thread safe.
    volatile size_t m_rIndex;   
    volatile size_t m_wIndex;
};

#endif // RINGBUF_H_
