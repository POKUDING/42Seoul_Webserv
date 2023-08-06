#include "../../includes/util/InputBuffer.hpp"
#include <iostream>
// constructor and destructor

InputBuffer::InputBuffer() : mIndex(0) { }
InputBuffer::~InputBuffer() { }

// member functions

// public

void    InputBuffer::reset()
{
    clear();
    mIndex = 0;
}

void    InputBuffer::reset(size_t index)
{
    string temp = substr(index, size());
    clear();
    append(temp.c_str(), temp.size());
    mIndex = 0;
}

char*   InputBuffer::getCharPointer() // c_str() 대신
{
    return const_cast<char*>(c_str()) + mIndex;
}

void    InputBuffer::updateIndex() // clear() 대신
{
    mIndex = size();
}

void    InputBuffer::updateIndex(size_t mIndex) // 필요할 때
{
    this->mIndex = mIndex;
}

// getters

size_t  InputBuffer::getIndex() { return mIndex; }
