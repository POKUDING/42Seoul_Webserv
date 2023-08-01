#pragma once
#ifndef RCGI_HPP
#define RCGI_HPP

#include "ARequest.hpp"
#include "../socket/Body.hpp"

class RCgi: public ARequest
{
	public:
		RCgi(string mRoot, map<string, string> header_key_val, vector<Server>* servers);
		virtual ~RCgi();

		virtual pid_t			operate();
		virtual	const string	createResponse();

		const Body&		getBody() const;

	private:
		void	executeCgi();
		string	getRequestMethod();
};

#endif // RCGI_HPP