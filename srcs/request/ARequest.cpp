#include "../../includes/request/ARequest.hpp"

// constructors and destructor

ARequest::ARequest(string root, int mType, map<string, string> header_key_val, vector<Server>* servers)
			: mType(mType)
{
	mBasics.host = header_key_val["Host"];
	mBasics.user_agent = header_key_val["User-Agent"];
	mBasics.content_length = atoi(header_key_val["Content-Length"].c_str());
	mBasics.content_type = header_key_val["Content-Type"];
	mBasics.content_disposition = header_key_val["Content-Disposition"];
	mBasics.transfer_encoding = header_key_val["Transfer-encoding"];

	//method 필수요소 확인: host, user_agent
	if (mBasics.host.size() == 0 || mBasics.user_agent.size() == 0)
		throw runtime_error("Bad request:: no must header");
	//method 필수요소 확인: connection
	if (header_key_val["Connection"] == "keep-alive") {
		mBasics.connection = KEEP_ALIVE;
	} else if (header_key_val["Connection"] == "close") {
		mBasics.connection = CLOSE;
	} else {
		throw runtime_error("Bad request:: connection");
	}
	//method 필수요소 확인: content_length 확인 (숫자인지 아닌지 여부)
	if (mBasics.content_length == 0 && header_key_val["Content-Length"].size()) {
		throw runtime_error("Bad request:: content-length");
	}

	//초기화
	mRoot = root;
	mServer = findServer(servers);
	findLocation(mServer);
	mSendLen = 0;

	//요청한 file/dir을 실제 location/server block root 주소에 따라 변경
	string subDir = mRoot.substr(mLocation.getKey().size(), mRoot.size() - mLocation.getKey().size());
	mRoot = mLocation.getRoot();
	if (mRoot.size() == 0)
		mRoot = mServer.getRoot() + mLocation.getKey();
	mRoot += subDir;
	//존재 확인
	if (access(mRoot.c_str(), F_OK) < 0)
		runtime_error("Error: requested dir/file not available.");
	//file vs dir
	DIR* dir = opendir(mRoot.c_str());
	if (dir) {
		mIsFile = false;
		closedir(dir);
	} else {
		mIsFile = true;
	}
}

ARequest::ARequest(int mType) : mRoot(""), mType(mType) {}
ARequest::~ARequest() { }

// member functions

// public

void	ARequest::setCode(int code) { this->mCode = code; }

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

void	ARequest::findRootLocation(Server& server, string root)
{
	size_t		find_len = 0;

	for(int loc_idx = 0, end = server.getLocation().size(); loc_idx < end; ++loc_idx)
	{
		if (root.substr(0, server.getLocation()[loc_idx].getKey().size()) == server.getLocation()[loc_idx].getKey() && \
			server.getLocation()[loc_idx].getKey().size() > find_len)
			{
				mLocation = server.getLocation()[loc_idx];
				find_len = server.getLocation()[loc_idx].getKey().size();
			}
	}
	if (find_len == 0)
		findRootLocation(server, "/");
}

int	ARequest::findExtentionLocation(Server& server)
{
	string extention;

	size_t	pos = mRoot.rfind('.');
	if (pos == string::npos || mRoot.find('/', pos) != string::npos || pos == mRoot.size())
		return 1;
	extention = mRoot.substr(pos);
	for(int loc_idx = 0, end = server.getLocation().size(); loc_idx < end; ++loc_idx)
	{
		if (extention == server.getLocation()[loc_idx].getKey()) {
			mLocation = server.getLocation()[loc_idx];
			return 0;
		}
	}
	return 1;
}

void	ARequest::findLocation(Server& server)
{
	if (findExtentionLocation(server))
		findRootLocation(server, mRoot);
}

void	ARequest::setPipe()
{
	if (pipe(mPipe) < 0)
		throw runtime_error("Error: pipe create failed");
}

// void	ARequest::createErrorRequest(int code)
// {
// 	ARequest* toDelete = this;
// 	this = new RBad(code);
// 	delete toDelete;
// }

// getters

int					ARequest::getType() const { return mType; }
const string&		ARequest::getRoot() const { return mRoot; }
const t_basic&		ARequest::getBasics() const { return mBasics; }
// const t_response&	ARequest::getResponse() const { return mResponse; }