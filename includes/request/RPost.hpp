#pragma once
#ifndef RPOST_HPP
#define RPOST_HPP

#include "ARequest.hpp"

class RPost: public ARequest
{
	public:
		RPost(string mRoot, map<string, string> header_key_val);
		virtual ~RPost();

		virtual	const string	createResponse();

		const string&	getBody() const;
		void			setBody(string mBody);

	private:
		string	mBody;
};

#endif //RPOST_HPP