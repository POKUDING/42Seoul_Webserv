#pragma once
#ifndef RDELETE_HPP
#define RDELETE_HPP

#include "ARequest.hpp"

class RDelete: public ARequest
{
	public:
		RDelete(string mRoot, map<string, string> header_key_val);
		virtual ~RDelete();

		virtual	const string&	createResponse();

	private:
		// virtual void	parse(map<string, string> header_key_val);
};

#endif //RDELETE_HPP