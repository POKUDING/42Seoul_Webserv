#pragma once
#ifndef RBAD_HPP
#define RBAD_HPP

#include "ARequest.hpp"

class RBad: public ARequest
{
	public:
		RBad(int code, map<string, string> header_key_val, vector<Server>* servers, int type = BAD);
		RBad(int code, Server server);
		virtual ~RBad();

		virtual pid_t			operate();
		void					createErrorResponse(int code);
		virtual	const string	createResponse();
	private:
		string	mMSG;
};

#endif // RBAD_HPP