#pragma once
#ifndef TIME_HPP
#define TIME_HPP

#include <iostream>
#include <ostream>
#include <string>
#include <ctime>

using namespace std;

#define TIME_SIZE 40

class Time
{
    private:
        Time(const Time& src);
        Time& operator=(Time const& rhs);
        Time();
        ~Time();

    public:
        // static const string& stamp();
        static void         stamp(char *timeStamp);
        static std::time_t  stamp();
        // char timeStamp[TIME_SIZE];
};

#endif // TIME_HPP