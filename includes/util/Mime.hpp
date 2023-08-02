#pragma once
#ifndef MIME_HPP
#define MIME_HPP

#include <map>
#include <string>

class Mime : public std::map<std::string, std::string>
{
	public:
		Mime();
		~Mime();
};

#endif // MIME_HPP