#pragma once
#ifndef AREQUEST_HPP
#define AREQUEST_HPP

class ARequest
{
	public:
		ARequest(/* args */);
		virtual ~ARequest();
		virtual void response() = 0;
	// private:
	// 	/* data */
};

#endif //AREQUEST_HPP