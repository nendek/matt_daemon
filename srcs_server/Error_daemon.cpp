#include "Error_daemon.hpp"

Error::Error( int num_error, std::string str_error, int lvl_error ) throw():
	_num_error(num_error), _str_error(str_error), _lvl_error(lvl_error)
{
	return;
}

Error::Error( void ) throw(): _num_error(0), _str_error("default"), _lvl_error(0)
{
	return;
}

Error::~Error( void ) throw()
{
	return;
}

Error::Error ( Error const & src ) throw()
{
	*this = src;
	return;
}

Error & Error::operator=( Error const & rhs ) throw()
{
	if (&rhs != this)
	{
		this->_num_error = rhs._num_error;
		this->_str_error = rhs._str_error;
		this->_lvl_error = rhs._lvl_error;
	}
	return (*this);
}

int		Error::getLvl( void ) const throw()
{
	return (this->_lvl_error);
}


const char*	Error::what( void ) const throw()
{
	return (this->_str_error.c_str());
}
