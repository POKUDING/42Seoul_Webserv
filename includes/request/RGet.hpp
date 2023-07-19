#pragma once
#ifndef RGET_HPP
#define RGET_HPP

#include "ARequest.hpp"

class RGet : public ARequest
{
	public:
		RGet(void);
		RGet(const RGet& src);
		virtual ~RGet(void);
		RGet& operator=(RGet const& rhs);
	private:
};

#endif //RGET_HPP
