#include "../../includes/request/ARequest.hpp"

// constructors and destructor

ARequest::ARequest(string root, int mType, map<string, string> header_key_val, vector<Server>* servers)
			: mCode(0), mType(mType), mReadPipe(0), mWritePipe(0), mSendLen(0)
{
	memset((void *)&mBasics, 0, sizeof(mBasics));
	mBasics.host = header_key_val["Host"];
	if (mBasics.host.find(':') != string::npos)
		mBasics.host = mBasics.host.substr(0,mBasics.host.find(':'));
	mBasics.user_agent = header_key_val["User-Agent"];
	mBasics.content_length = SpiderMenUtil::atoi(header_key_val["Content-Length"].c_str());
	mBasics.content_type = header_key_val["Content-Type"];
	mBasics.content_disposition = header_key_val["Content-Disposition"];
	mBasics.transfer_encoding = header_key_val["Transfer-Encoding"];
	mBasics.x_secret = header_key_val["X-Secret-Header-For-Test"];

	switch(mType) {
		case GET: mMethod = "GET"; break;
		case POST: mMethod = "POST"; break;
		case DELETE: mMethod = "DELETE"; break;
		case PUT: mMethod = "PUT"; break;
		case BAD: mMethod = "BAD"; break;
		case HEAD: mMethod = "HEAD"; break;
		default: break;
	}

	//method 필수요소 확인: host, user_agent
	if (mBasics.host.size() == 0 || mBasics.user_agent.size() == 0)
		throw 400;
	//method 필수요소 확인: connection
	if (header_key_val["Connection"] == "close")
		mBasics.connection = "close";
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

	findRootLocation(mServer, mRoot);
	if (mType != PUT)
		findExtensionLocation(mServer);

	//요청한 file/dir을 실제 location/server block root 주소에 따라 변경
	if (mLocation.getRoot().size())//location 블럭에 루트가 있는 경우
		mRoot.replace(mRoot.find(mLocation.getKey()), mLocation.getKey().size(), mLocation.getRoot());
	else
		mRoot = mServer.getRoot() + "/" + mRoot;

	//존재 확인
	if (mType != POST && mType != PUT && !mLocation.getRedirect().size() && access(mRoot.c_str(), F_OK) < 0)
	{
		// cout << "404 not found? : "<< mRoot.c_str() << endl;
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

	//maxbodysize setting
	if (mLocation.getLocationMaxBodySize() < 0)
		mBody.setMaxBodySize(mServer.getClientMaxBodySize());
	else
		mBody.setMaxBodySize(mLocation.getLocationMaxBodySize());
}

ARequest::ARequest(int mType, map<string, string> header_key_val, vector<Server>* servers) : mRoot(""), mType(mType), mReadPipe(0), mWritePipe(0), mSendLen(0)
{
	mBasics.host = header_key_val["Host"];
	mServer = findServer(servers);
}

ARequest::ARequest(int mType, Server mServer) : mRoot(""), mType(mType), mServer(mServer), mReadPipe(0), mWritePipe(0), mSendLen(0) { }
ARequest::~ARequest() { }

// member functions

// public

// getters and setters

int					ARequest::getReadPipe() { return mReadPipe; }
int					ARequest::getWritePipe() { return mWritePipe; }
string&				ARequest::getPipeValue() { return mPipeValue; }
size_t				ARequest::getSendLen() const { return mSendLen; }
int					ARequest::getCode() const { return mCode; }
int					ARequest::getType() const { return mType; }
Server				ARequest::getServer() { return mServer; }
const string&		ARequest::getRoot() const { return mRoot; }
const t_basic&		ARequest::getBasics() const { return mBasics; }
Body&				ARequest::getBody() { return mBody; }
const string&		ARequest::getCgiPath() const { return mCgiPath; }
const string&		ARequest::getCgiBin() const { return mCgiBin; }
void				ARequest::addSendLen(size_t len) { mSendLen += len; }
void				ARequest::setCode(int code) { this->mCode = code; }

// protected

Server	ARequest::findServer(vector<Server>* servers)
{
	for (int server_idx = 0,server_end = servers->size(); server_idx < server_end; ++server_idx) {
		for (int name_idx = 0, name_end = (*servers)[server_idx].getServerName().size(); name_idx < name_end; ++name_idx) {
			if (mBasics.host == (*servers)[server_idx].getServerName()[name_idx])
				return (*servers)[server_idx];
		}
	}
	return (*servers)[0];
}

// void	ARequest::findLocation(Server& server)
// {
// 	// 왜 함수가 두개로 나누어져 있는지?

// 	//Find Root Location으로 루트 설정
// 	findRootLocation(server, mRoot);

// 	//Find extension location으로 cgi path 업데이트 해주기 (있는 경우에만)
// 	findExtensionLocation(server);// put은 제외
// }

void	ARequest::findRootLocation(Server& server, string root)
{
	size_t		find_len = 0;

	for(int loc_idx = 0, end = server.getLocation().size(); loc_idx < end; ++loc_idx) {
		if (root.substr(0, server.getLocation()[loc_idx].getKey().size()) == server.getLocation()[loc_idx].getKey() && \
			server.getLocation()[loc_idx].getKey().size() > find_len) {

			mLocation = server.getLocation()[loc_idx];
			find_len = server.getLocation()[loc_idx].getKey().size();
			if (mType != GET)
				mCgiPath = server.getLocation()[loc_idx].getCgiPath();
		}
	}
	if (find_len == 0)
		findRootLocation(server, "/");
}

void	ARequest::findExtensionLocation(Server& server)
{
	string extension;

	size_t	pos = mRoot.rfind('.');
	if (pos == string::npos || mRoot.find('/', pos) != string::npos || pos == mRoot.size())
		return;
	extension = mRoot.substr(pos);
	for (size_t loc_idx = 0, end = server.getLocation().size(); loc_idx < end; ++loc_idx) {
		if (extension == server.getLocation()[loc_idx].getKey()) {
			if (!server.getLocation()[loc_idx].getLimitExcept().size()) {
				mCgiPath = server.getLocation()[loc_idx].getCgiPath();
				return;
			}
			for (int LimitExceptIdx = 0, end = server.getLocation()[loc_idx].getLimitExcept().size(); LimitExceptIdx < end; ++LimitExceptIdx) {
				if (server.getLocation()[loc_idx].getLimitExcept()[LimitExceptIdx] == mMethod) {
					mCgiPath = server.getLocation()[loc_idx].getCgiPath();
					return;
				}
			}
		}
	}
}

void	ARequest::setPipe(int *fd)
{
	if (pipe(fd) < 0)
		throw 500;// throw runtime_error("Error: pipe create failed");
}

void	ARequest::setCgiEnv()
{
	setenv("SERVER_NAME", getBasics().host.c_str(), 1);
	setenv("DOCUMENT_ROOT", mRoot.c_str(), 1);
	setenv("HTTP_USER_AGENT", getBasics().user_agent.c_str(), 1);
	setenv("SERVER_SOFTWARE", SPIDER_SERVER, 1); // "\r\n 빼야하나?"
	setenv("REQUEST_METHOD", mMethod.c_str(), 1);
	setenv("SERVER_PORT", SpiderMenUtil::itostr(mServer.getListen()).c_str(), 1);
	setenv("SERVER_PROTOCOL", "HTTP/1.1", 1);
	setenv("QUERY_STRING", mQuery.c_str(), 1);
	setenv("PATH_INFO", getCgiPath().c_str(), 1);
	setenv("HTTP_X_SECRET_HEADER_FOR_TEST", getBasics().x_secret.c_str(), 1);

	if (mType == PUT) {
		if (getBasics().content_type.size())
			setenv("CONTENT_TYPE", getBasics().content_type.c_str(), 1);
		else {
			string type = "text/plain";
			setenv("CONTENT_TYPE", type.c_str(), 1);
		}
		setenv("CONTENT_LENGTH", SpiderMenUtil::itostr(mBody.getBody().size()).c_str(), 1);
	} else if (mBody.getChunked()) {
		string	type = "chunked";
		setenv("HTTP_TRANSFER_ENCODING", type.c_str(), 1);
	} else {
		setenv("CONTENT_TYPE", getBasics().content_type.c_str(), 1);
		setenv("CONTENT_LENGTH", SpiderMenUtil::itostr(getBasics().content_length).c_str(), 1);
	}
	
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
	// cout << "mQuery : " << mQuery << "mRoot : " << mRoot << endl;
}

void	ARequest::setType(int type) { mType = type; }
