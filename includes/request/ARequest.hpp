#pragma once
#ifndef AREQUEST_HPP
#define AREQUEST_HPP

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include "../basic.hpp"
#include "../util/Time.hpp"
#include "../socket/Body.hpp"

using namespace std;

typedef struct	s_basic 
{
	//must for METHOD
	string	host;//Host: 
	int		connection; // default: keep-alive
	string	user_agent;	//우선 없으면 bad request로 처리
	// string	accept;		//크롬이면 기본으로 다 되니까 필요없을듯?
	// string	accept_encoding;	//필요 없을듯?
	// string	accept_language;	//필요 없을듯?
	
	//must for POST
	size_t	content_length;//Content-Length:
	string	content_type;//Content-Type:	// if (multipart/form-data) => boundary 데이터 필요
	string	boundary;	//(in Content-Type) body 부분 구분자
	string	transfer_encoding;//Transfer-encoding << chunked 여부가 이 태그로 옵니당
	//하기 두개는 Body에서 확인 가능함
	string	content_disposition;	//(in boundary) Content-Disposition: 
	string	filename;					//(in Content-Disposition) 저장할 filename(확장자명까지)
	//must for GET

	//must for DELETE

} t_basic;

// typedef struct	s_response
// {
// 	//same everytime => define으로 관리하자 -> basic.hpp에 저장해둠
// 	//http version	===> HTTP/1.1
// 	//server		===> Server: SpiderMen/1.0.0 (Linux)
// 	//content-type	===> Content-Type: text/html; charset=UTF-8

// 	//must for METHOD (date는 바로 추출해서 넣어주자)
// 	int		code;		//200, 400, 500
// 	string	status;		//OK, Bad Request, Internal Server Error
// 	int		content_length;	//Content-Length:
// 	string	content_type;//	===> Content-Type: text/html; charset=UTF-8  // image/png
	
// 	//must for POST
	
// 	//must for GET

// 	//must for DELETE
	
// } t_response;

class ARequest
{
	public:
		virtual	~ARequest();
		ARequest(string mRoot, int mType, map<string, string> header_key_val);
		ARequest(int mType);

		int					getType() const;
		const string&		getRoot() const;
		const t_basic&		getBasics() const;
		// const t_response&	getResponse() const;

		virtual	const string	createResponse() = 0;

		size_t			mSendLen;

		Body			mBody;
	protected:
		const string	mRoot;
		const int		mType;
		t_basic			mBasics;
		// t_response		mResponse;

		
};

#endif //AREQUEST_HPP