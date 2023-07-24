#pragma once
#ifndef AREQUEST_HPP
#define AREQUEST_HPP

#include <map>
#include <string>
#include <sstream>
#include "../basic.hpp"
#include "../util/Time.hpp"

using namespace std;

typedef struct	s_basic 
{
	//must for METHOD
	string	host;
	string	connection;
	string	user_agent;
	string	accept;
	string	accept_encoding;
	string	accept_language;
	
	//must for POST
	string content_length;//Content-Length;
	string content_type;//Content-Type;
	string transfer_encoding;//Transfer-encoding; << chunked 여부가 이 태그로 옵니당
	//must for GET

	//must for DELETE

} t_basic;

typedef struct	s_response
{
	//same everytime => define으로 관리하자 -> basic.hpp에 저장해둠
	//http version	===> HTTP/1.1
	//server		===> Server: SpiderMen/1.0.0 (Linux)
	//content-type	===> Content-Type: text/html; charset=UTF-8

	//must for METHOD (date는 바로 추출해서 넣어주자)
	int		code;		//200
	string	status;		//OK
	int		content_length;	//Content-Length:
	
	//must for POST
	
	//must for GET

	//must for DELETE
	
} t_response;

class ARequest
{
	public:
		virtual	~ARequest();
		ARequest(string mRoot, int mType, map<string, string> header_key_val);

		int					getType() const;
		const string&		getRoot() const;
		const t_basic&		getBasics() const;
		const t_response&	getResponse() const;
		const string&		getMSG() const;

		void				setMSG(const string& msg);

		virtual	const string&	createResponse() = 0;

		string			mMSG;
		char 			timeStamp[TIME_SIZE];
		size_t			mSendLen;

	private:
		const string	mRoot;
		const int		mType;
		t_basic			mBasics;
		t_response		mResponse;

		// virtual	void	parse(map<string, string> header_key_val) = 0;
};

#endif //AREQUEST_HPP