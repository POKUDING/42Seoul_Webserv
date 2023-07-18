#pragma once
#ifndef RDELETE_HPP
#define RDELETE_HPP

#include "ARequest.hpp"

class RDelete: public ARequest
{
	public:
		RDelete(/* args */);
		~RDelete();
	private:
		/* data */
};

#endif //RDELETE_HPP