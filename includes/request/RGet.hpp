#pragma once
#ifndef RGET_HPP
#define RGET_HPP

#include "ARequest.hpp"

class RGet : public ARequest
{
	public:
		RGet(string mRoot, map<string, string> header_key_val);
		virtual ~RGet();

		virtual	const string&	createResponse();
		
	private:
		// virtual void	parse(map<string, string> header_key_val);
};

#endif //RGET_HPP
