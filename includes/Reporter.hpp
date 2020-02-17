#ifndef	_REPORTER_HPP_
# define _REPORTER_HPP_

# include <string>
# include <fstream>
# include <locale>
# include <ctime>
# include <iostream>

# include "Error_daemon.hpp"

enum logLevel { error, info, msg };

class Tintin_reporter
{
	public:
		Tintin_reporter( std::string pathname );
		~Tintin_reporter( void );
		Tintin_reporter( Tintin_reporter const & src );
		Tintin_reporter & operator=( Tintin_reporter const & src);

		std::string		getPathname( void ) const;

		void			log( logLevel type, std::string msg );
		
	private:
		std::string	formatTimeNow( void ) const;

		std::string	_pathname;
		std::ofstream	_fs;

		Tintin_reporter( void );
};

#endif
