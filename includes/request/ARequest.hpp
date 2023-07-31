#pragma once
#ifndef AREQUEST_HPP
#define AREQUEST_HPP

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <dirent.h>
#include "../config/Server.hpp"
#include "../basic.hpp"
#include "../util/Time.hpp"
#include "../util/SpiderMenUtil.hpp"
#include "../socket/Body.hpp"
// #include "./RBad.hpp"

extern	char** env;
// class	RBad;

using namespace std;

typedef struct	s_basic 
{
	//must for METHOD
	string	host;//Host: 
	string	connection; // default: keep-alive
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

class ARequest
{
	public:
		ARequest(string Root, int mType, map<string, string> header_key_val, vector<Server>* servers);
		ARequest(int mType);
		virtual	~ARequest();

		virtual pid_t			operate() = 0;
		virtual	const string	createResponse() = 0;

		void				findLocation(Server& server);
		void				findRootLocation(Server& server, string root);
		void				findExtentionLocation(Server& server);
		// int					findExtentionLocation(Server& server);
		Server	 			findServer(vector<Server>* servers);
		void				setPipe();
		void				checkPipe();
		// void				createErrorRequest(int code);

		string&				getPipeValue();
		void				setCode(int code);
		int					getCode() const;
		int					getType() const;
		const string&		getRoot() const;
		const t_basic&		getBasics() const;
		Body&				getBody();
		string				getCgiPath() const;
		string				getCgiBin() const;

		size_t			mSendLen;
	protected:
		void			setCgiEnv();
		void			cutQuery();
		int				checkDeleteFile();

		bool			mIsFile;
		int				mCode;
		string			mRoot;
		string			mQuery;
		string			mPipeValue;
		string			mRequest;
		int				mType;
		Server			mServer;
		Body			mBody;
		Location		mLocation;
		t_basic			mBasics;
		int				mPipe[2];
		string			mCgiPath;
		string			mCgiBin;
};

#endif // AREQUEST_HPP