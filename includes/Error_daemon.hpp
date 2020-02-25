#ifndef _ERROR_DAEMON_HPP_
# define _ERROR_DAEMON_HPP_

# include <exception>
# include <string>

class Error: public std::exception
{
	public:
		Error( int num_error, std::string str_error, int lvl_error ) throw();
		Error( Error const & src ) throw();
		virtual ~Error( void ) throw();

		virtual const char*	what( void ) const throw();
		int			getLvl( void ) const throw();

	private:
		Error( void ) throw();
		Error & operator=( Error const & rhs ) throw();

		int		_num_error;
		std::string	_str_error;
		int		_lvl_error;
};

#endif
