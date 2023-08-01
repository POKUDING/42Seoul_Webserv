#include "../../includes/request/ARequest.hpp"

// constructors and destructor

ARequest::ARequest(string root, int mType, map<string, string> header_key_val, vector<Server>* servers)
			: mCode(0), mType(mType), mSendLen(0)
{
	memset((void *)&mBasics, 0, sizeof(mBasics));
	mBasics.host = header_key_val["Host"];
	mBasics.user_agent = header_key_val["User-Agent"];
	mBasics.content_length = atoi(header_key_val["Content-Length"].c_str());
	mBasics.content_type = header_key_val["Content-Type"];
	mBasics.content_disposition = header_key_val["Content-Disposition"];
	mBasics.transfer_encoding = header_key_val["Transfer-Encoding"];

	//method 필수요소 확인: host, user_agent
	if (mBasics.host.size() == 0 || mBasics.user_agent.size() == 0)
		throw 400;
	//method 필수요소 확인: connection
	if (header_key_val["Connection"] == "close")
		mBasics.connection = CLOSE;
	else
		mBasics.connection = KEEP_ALIVE;
	//method 필수요소 확인: content_length 확인 (숫자인지 아닌지 여부)
	// if (mBasics.content_length == 0 && header_key_val["Content-Length"].size()) {
	// 	throw 400;
	// }

	//초기화
	mRoot = root;

	cutQuery();
	mServer = findServer(servers);
	findLocation(mServer);

	//요청한 file/dir을 실제 location/server block root 주소에 따라 변경
	if (mLocation.getRoot().size())//location 블럭에 루트가 있는 경우
		mRoot.replace(mRoot.find(mLocation.getKey()), mLocation.getKey().size(), mLocation.getRoot());
	else
		mRoot = mServer.getRoot() + "/" + mRoot;

	//존재 확인
	if (mType != POST && !mLocation.getRedirect().size() && access(mRoot.c_str(), F_OK) < 0)
	{
		cout << "404 not found? : "<< mRoot.c_str() << endl;
		throw 404;
	}

	//file vs dir
	DIR* dir = opendir(mRoot.c_str());
	if (dir) {
		mIsFile = false;
		closedir(dir);
	} else {
		mIsFile = true;
	}
}

ARequest::ARequest(int mType) : mRoot(""), mType(mType), mSendLen(0) {}
ARequest::~ARequest() { }

// member functions

// public

void	ARequest::checkPipe()
{
	char	buff[1024];
	string	readvalue;
	int		readlen = 1;

	while (readlen > 0)
	{
		cout << "start" << endl;
		readlen = read(mPipe[0], buff, 1024);
		if (readlen > 0)
			readvalue.append(buff, readlen);
		cout << "fin " << buff << endl;
	}
	close(mPipe[0]);
	readvalue = SpiderMenUtil::replaceCRLF(readvalue);
	if (readvalue.find("Content-Type:") == string::npos)
	{
		size_t pos = readvalue.find("\r\n\r\n");
		if (pos == string::npos)
			readvalue = "\r\n" + readvalue;
		readvalue = "Content-Type: text/html; charset=UTF-8\r\n" + readvalue;
	}
	// cout << readvalue << endl;
	mPipeValue = readvalue;
}

// getters and setters

string&				ARequest::getPipeValue() { return mPipeValue; }
size_t				ARequest::getSendLen() const { return mSendLen; }
int					ARequest::getCode() const { return mCode; }
int					ARequest::getType() const { return mType; }
const string&		ARequest::getRoot() const { return mRoot; }
const t_basic&		ARequest::getBasics() const { return mBasics; }
Body&				ARequest::getBody() { return mBody; }
string				ARequest::getCgiPath() const { return mCgiPath; }
string				ARequest::getCgiBin() const { return mCgiBin; }
void				ARequest::addSendLen(size_t len) { mSendLen += len; }
void				ARequest::setCode(int code) { this->mCode = code; }

// protected

Server	ARequest::findServer(vector<Server>* servers)
{
	for (int server_idx = 0,server_end = servers->size(); server_idx < server_end; ++server_idx)
	{
		for (int name_idx = 0, name_end = (*servers)[server_idx].getServerName().size(); name_idx < name_end; ++name_idx)
			if (mBasics.host == (*servers)[server_idx].getServerName()[name_idx])
				return (*servers)[server_idx];
	}
	return (*servers)[0];
}

void	ARequest::findLocation(Server& server)
{
	
	//Find Root Location으로 루트 설정
	findRootLocation(server, mRoot);
	
	//Find extension location으로 cgi path 업데이트 해주기 (있는 경우에만)
	findExtensionLocation(server);

	//이 아래가 원본
	// if (findExtensionLocation(server))
	// 	findRootLocation(server, mRoot);
}

void	ARequest::findRootLocation(Server& server, string root)
{
	size_t		find_len = 0;

	for(int loc_idx = 0, end = server.getLocation().size(); loc_idx < end; ++loc_idx)
	{
		if (root.substr(0, server.getLocation()[loc_idx].getKey().size()) == server.getLocation()[loc_idx].getKey() && \
			server.getLocation()[loc_idx].getKey().size() > find_len) {
			
			mLocation = server.getLocation()[loc_idx];
			find_len = server.getLocation()[loc_idx].getKey().size();

			mCgiPath = server.getLocation()[loc_idx].getCgiPath();
			mCgiBin = server.getLocation()[loc_idx].getCgiBin();
			// cout << "UPDATED" << endl;
		}
	}
	if (find_len == 0)
		findRootLocation(server, "/");

	// cout << "find location: " << mLocation.getKey() << endl;
}

void	ARequest::findExtensionLocation(Server& server)
{
	string extension;

	size_t	pos = mRoot.rfind('.');
	if (pos == string::npos || mRoot.find('/', pos) != string::npos || pos == mRoot.size())
		return;
	extension = mRoot.substr(pos);
	for(size_t loc_idx = 0, end = server.getLocation().size(); loc_idx < end; ++loc_idx)
	{
		if (extension == server.getLocation()[loc_idx].getKey()) {
			if (!mCgiBin.size())
				mCgiBin = server.getLocation()[loc_idx].getCgiBin();
			if (!mCgiPath.size())
				mCgiPath = server.getLocation()[loc_idx].getCgiPath();
			
			cerr << "FE: cgi bin: " << mCgiBin << endl;
			cerr << "FE: cgi path: " <<mCgiPath << endl;

			return;
		}
	}
}

//원본 코드 ======
// int	ARequest::findExtensionLocation(Server& server)
// {
// 	string extension;

// 	size_t	pos = mRoot.rfind('.');
// 	if (pos == string::npos || mRoot.find('/', pos) != string::npos || pos == mRoot.size())
// 		return 1;
// 	extension = mRoot.substr(pos);
// 	for(int loc_idx = 0, end = server.getLocation().size(); loc_idx < end; ++loc_idx)
// 	{
// 		if (extension == server.getLocation()[loc_idx].getKey()) {
// 			mLocation = server.getLocation()[loc_idx];
// 			return 0;
// 		}
// 	}
// 	return 1;
// }

void	ARequest::setPipe()
{
	if (pipe(mPipe) < 0)
		throw runtime_error("Error: pipe create failed");
}

void	ARequest::setCgiEnv()
{
	setenv("SERVER_NAME", getBasics().host.c_str(), 1);
	setenv("DOCUMENT_ROOT", mRoot.c_str(), 1);
	setenv("HTTP_USER_AGENT", getBasics().user_agent.c_str(), 1);
	setenv("SERVER_SOFTWARE", SPIDER_SERVER, 1); // "\r\n 빼야하나?"
	setenv("REQUEST_METHOD", mRequest.c_str(), 1);
	setenv("SERVER_PORT", to_string(mServer.getListen()).c_str(), 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("QUERY_STRING", mQuery.c_str(), 1);
	setenv("PATH_INFO", getCgiPath().c_str(), 1);

	if (mBody.getChunked()) {
		string	type = "text/plain";
		int		length = getBody().getBody().size();
		setenv("CONTENT_TYPE", type.c_str(), 1);
		setenv("CONTENT_LENGTH", SpiderMenUtil::itostr(length).c_str(), 1);
	} else {
		setenv("CONTENT_TYPE", getBasics().content_type.c_str(), 1);
		setenv("CONTENT_LENGTH", SpiderMenUtil::itostr(getBasics().content_length).c_str(), 1);
	}
	//밑 두 줄 원본임
	// setenv("CONTENT_TYPE", getBasics().content_type.c_str(), 1);
	// setenv("CONTENT_LENGTH", SpiderMenUtil::itostr(getBasics().content_length).c_str(), 1);
	

	// // SERVER_NAME
	// // SERVER_SOFTWARE // CGI 프로그램 이름 및 버전 ex) 아파치 / 2.2.14
	// SERVER_PROTOCOL // ??? HTTP/1.1 ?
	// // SERVER_PORT // ???
	// // REQUEST_METHOD // GET POST ..
	// // PATH_INFO // CGI path /wevsrv/cgi_bin/example.cgi
	// DOCUMENT_ROOT // ???
	// QUERY_STRING // url?key=value&key=value ..
	// REMOTE_HOST	//client host name 없으면 정의 x
	// REMOTE_ADDR	//client ip
	// // CONTENT_TYPE
	// // CONTENT_LENGTH
	// HTTP_REFERER // ????
	// // HTTP_USER_AGENT
}

void	ARequest::cutQuery()
{
	size_t pos = mRoot.rfind("?");
	if (pos == string::npos || mRoot.find("/", pos) != string::npos)
	{
		// cout << "false ? " << endl;
		return;
	}
	mQuery = mRoot.substr(pos + 1);
	mRoot = mRoot.substr(0, pos);
	cout << "mQuery : " << mQuery << "mRoot : " << mRoot << endl;
}
