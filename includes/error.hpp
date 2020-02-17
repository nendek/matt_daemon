#ifndef _ERROR_HPP_
# define _ERROR_HPP_

# include <exception>
#include <string>

class Error: public std::exception
{
	public:
		Error( int num_error, std::string str_error, int lvl_error ) throw();
		virtual ~Error( void ) throw();

		virtual const char*	what( void ) const throw();
		int			getLvl( void ) const throw();

	private:
		int		_num_error;
		std::string	_str_error;
		int		_lvl_error;
};

#endif
