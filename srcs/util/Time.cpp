#include "../../includes/util/Time.hpp"

void Time::stamp(char *timeStamp)
{
    std::time_t now = std::time(NULL);
    memset(timeStamp, 0, TIME_SIZE);
    // char timeStamp[TIME_SIZE];
    std::strftime(timeStamp, TIME_SIZE, "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", std::localtime(&now));
    // return timeStamp;
}
