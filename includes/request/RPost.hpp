#pragma once
#ifndef RPOST_HPP
#define RPOST_HPP

#include "ARequest.hpp"
#include "../socket/Body.hpp"

class RPost: public ARequest
{
	public:
		RPost(string mRoot, map<string, string> header_key_val, vector<Server>* servers);
		virtual ~RPost();

		virtual pid_t			operate();
		virtual	const string	createResponse();

		const Body&		getBody() const;

	private:
		void	executeCgi();
		string	getRequestMethod();
};

#endif // RPOST_HPP