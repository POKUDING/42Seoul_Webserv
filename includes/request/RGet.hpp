#pragma once
#ifndef RGET_HPP
#define RGET_HPP

#include <dirent.h>
#include <algorithm>
#include "ARequest.hpp"
#include "../util/Mime.hpp"

using namespace std;

class RGet : public ARequest
{
	public:
		RGet(string mRoot, map<string, string> header_key_val, vector<Server>* servers);
		virtual ~RGet();

		virtual pid_t			operate();
		virtual	const string	createResponse();

	private:
		void			executeCgi();
		const string 	createRedirectResponse();
		const string	createLegacyResponse();
		const string	createCgiResponse();
};

#endif // RGET_HPP
