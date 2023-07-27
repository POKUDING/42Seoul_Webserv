#pragma once
#ifndef RGET_HPP
#define RGET_HPP

#include "ARequest.hpp"

class RGet : public ARequest
{
	public:
		RGet(string mRoot, map<string, string> header_key_val, vector<Server>* servers);
		virtual ~RGet();

		virtual pid_t			operate();
		virtual	const string	createResponse();
};

#endif // RGET_HPP
