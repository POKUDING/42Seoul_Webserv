#pragma once
#ifndef ASOCKET_HPP
#define ASOCKET_HPP

class ASocket
{
	public:
		ASocket(void);
		ASocket(const ASocket& src);
		virtual ~ASocket(void);
		ASocket& operator=(ASocket const& rhs);

	protected:
		

};

#endif // ASOCKET_HPP