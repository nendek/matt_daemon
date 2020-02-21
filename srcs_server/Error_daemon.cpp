#include "Error_daemon.hpp"

Error::Error( int num_error, std::string str_error, int lvl_error ) throw():
	_num_error(num_error), _str_error(str_error), _lvl_error(lvl_error)
{
	return;
}

Error::~Error( void ) throw()
{
	return;
}

int		Error::getLvl( void ) const throw()
{
	return (this->_lvl_error);
}


const char*	Error::what( void ) const throw()
{
	return (this->_str_error.c_str());
}
