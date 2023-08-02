#include "../../includes/util/SpiderMenUtil.hpp"

// vector<string>	SpiderMenUtil::splitString(string str, string del)
// {
// 	size_t	pos;
// 	size_t	before = 0;
// 	vector<string>	rtn;

// 	for(size_t i = 0; i < str.size(); i++)
// 	{
// 		pos = del.find(str[i]);
// 		if (pos == string::npos)
// 			continue ;
// 		if (i != before)
// 			rtn.push_back(str.substr(before, (i - before)));
// 		before = i + 1;
// 	}
// 	if (str.size() != before)
// 		rtn.push_back(str.substr(before, (str.size() - before)));
// 	return rtn;
// }

vector<string>	SpiderMenUtil::splitString(string str)
{
	size_t	before = 0;
	vector<string>	rtn;

	for(size_t i = 0; i < str.size(); i++)
	{
		if (!isspace(str[i]))
			continue ;
		if (i != before)
			rtn.push_back(str.substr(before, (i - before)));
		before = i + 1;
	}
	if (str.size() != before)
		rtn.push_back(str.substr(before, (str.size() - before)));
	return rtn;
}

vector<string>	SpiderMenUtil::splitString(string str, string del)
{
	size_t	pos = 0;
	size_t	before = 0;
	vector<string>	rtn;
	while (1)
	{
		pos = str.find(del.c_str(), before);
		if (pos != string::npos) {
			rtn.push_back(str.substr(before, pos - before));

		} else {
			if (str.size() != before)
				rtn.push_back(str.substr(before, str.size() - before));
			break ;
		}
		before = pos + del.size();
	}
	return rtn;
}

string	SpiderMenUtil::itostr(int num)
{
	stringstream 	stream;
	string			rtn;

	stream << num;
	stream >> rtn;

	return rtn;
}

string SpiderMenUtil::replaceCRLF(const string& input)
{
    string result;
    size_t startPos = 0;
    size_t foundPos = input.find('\n');

    if (foundPos == 0) {
        result += "\r\n";
        startPos = foundPos + 1;
        foundPos = input.find('\n', startPos);
    }

    while (foundPos != string::npos) {
        if (input[foundPos - 1] != '\r') {
            result += input.substr(startPos, foundPos - startPos);
            result += "\r\n";
        }
        startPos = foundPos + 1;
        foundPos = input.find('\n', startPos);
    }

    result += input.substr(startPos);

    return result;
}

std::vector<std::pair<std::string, std::string> > parseQueryString(const std::string& query) {
    std::vector<std::pair<std::string, std::string> > result;

    size_t pos = 0;
    while (pos < query.length()) {
        size_t equalPos = query.find('=', pos);
        if (equalPos == std::string::npos) {
            break; // '='을 찾지 못하면 종료
        }

        std::string key = query.substr(pos, equalPos - pos);
        pos = equalPos + 1;

        size_t ampPos = query.find('&', pos);
        if (ampPos == std::string::npos) {
            ampPos = query.length();
        }

        std::string value = query.substr(pos, ampPos - pos);
        pos = ampPos + 1;

        // URL 인코딩된 문자열을 원래 값으로 디코딩
        std::replace(value.begin(), value.end(), '+', ' '); // +를 공백으로 대체
        std::string decodedValue;
        for (size_t i = 0; i < value.length(); ++i) {
            if (value[i] == '%') {
                int hexValue;
                sscanf(value.substr(i + 1, 2).c_str(), "%x", &hexValue);
                decodedValue += static_cast<char>(hexValue);
                i += 2;
            } else {
                decodedValue += value[i];
            }
        }

        result.push_back(std::make_pair(key, decodedValue));
    }

    return result;
}