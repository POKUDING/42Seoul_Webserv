#pragma once
#ifndef RBAD_HPP
#define RBAD_HPP

#include "ARequest.hpp"

class RBad: public ARequest
{
	public:
		RBad(string mRoot, map<string, string> header_key_val);
		RBad(int code);
		virtual ~RBad();

		void					createErrorResponse(int code);
		virtual	const string	createResponse();
	private:
		string	mMSG;
};

#endif //RBAD_HPP