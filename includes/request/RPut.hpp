#pragma once
#ifndef RPUT_HPP
#define RPUT_HPP

#include "ARequest.hpp"
#include "../socket/Body.hpp"

class RPut: public ARequest
{
	public:
		RPut(string mRoot, map<string, string> header_key_val, vector<Server>* servers);
		virtual ~RPut();

		virtual pid_t			operate();
		virtual	const string	createResponse();

		const Body&		getBody() const;

	private:
		void	executeCgi();
		string	getRequestMethod();
};

#endif // RPUT_HPP