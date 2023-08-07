#pragma once
#ifndef INPUTBUFFER_HPP
#define INPUTBUFFER_HPP

#include <string>

using namespace std;

class InputBuffer : public string
{
    public:
        InputBuffer();
        ~InputBuffer();

        void            reset(); // 초기화
        void            reset(size_t index); // index부터 문자열 남기고 초기화
        char*           getCharPointer(); // c_str() + mIndex (c_str() 역할)
        void            updateIndex(); // mIndex = size() (claer() 역할)
        void            updateIndex(size_t mIndex); // this->mIndex = mIndex

        size_t          getIndex();

    private:
        size_t  mIndex;
};

#endif // INPUTBUFFER_HPP
