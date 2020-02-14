#include "reporter.hpp"

Tintin_reporter::Tintin_reporter( void ): _pathname("default pathname")
{
	return;
}

Tintin_reporter::Tintin_reporter( std::string pathname ): _pathname(pathname)
{
	return;
}

Tintin_reporter::~Tintin_reporter( void )
{
	return;
}

Tintin_reporter::Tintin_reporter( Tintin_reporter const & src )
{
	*this = src;
	return;
}

Tintin_reporter & Tintin_reporter::operator=( Tintin_reporter const & src )
{
	if (&src != this)
	{
	}
	return (*this);
}

std::string	Tintin_reporter::getPathname( void ) const
{
	return (this->_pathname);
}
