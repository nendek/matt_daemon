#ifndef REPORTER_HPP
# define REPORTER_HPP

# include <string>

class Tintin_reporter
{
	public:
		Tintin_reporter( std::string pathname );
		~Tintin_reporter( void );
		Tintin_reporter( Tintin_reporter const & src );
		Tintin_reporter & operator=( Tintin_reporter const & src);

		std::string		getPathname( void ) const;

	private:
		std::string _pathname;

		Tintin_reporter( void );
};

#endif
