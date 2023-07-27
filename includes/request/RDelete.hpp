#pragma once
#ifndef RDELETE_HPP
#define RDELETE_HPP

#include "ARequest.hpp"

class RDelete: public ARequest
{
	public:
		RDelete(string mRoot, map<string, string> header_key_val, vector<Server>* servers);
		virtual ~RDelete();

		virtual pid_t			operate();
		virtual	const string	createResponse();
};

#endif // RDELETE_HPP