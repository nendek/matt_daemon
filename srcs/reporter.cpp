#include "reporter.hpp"

Tintin_reporter::Tintin_reporter( void ): _pathname("default pathname")
{
	return;
}

Tintin_reporter::Tintin_reporter( std::string pathname ): _pathname(pathname)
{
	this->_fs.open(this->_pathname, std::ios::out | std::ios::app);
	if (!this->_fs.is_open())
		throw Error(1, "fail open file log", 2);
	return;
}

Tintin_reporter::~Tintin_reporter( void )
{
	this->_fs.close();
	return;
}

Tintin_reporter::Tintin_reporter( Tintin_reporter const & src )
{
	*this = src;
	return;
}

Tintin_reporter & Tintin_reporter::operator=( Tintin_reporter const & rhs )
{
	if (&rhs != this)
	{
		this->_pathname = rhs._pathname;
		this->_fs.copyfmt(rhs._fs);
		this->_fs.clear(rhs._fs.rdstate());
		this->_fs.basic_ios<char>::rdbuf(rhs._fs.rdbuf());
	}
	return (*this);
}

std::string	Tintin_reporter::getPathname( void ) const
{
	return (this->_pathname);
}

void		Tintin_reporter::log( logLevel type, std::string msg )
{
	std::string now = formatTimeNow();
	std::string msgType[3] = { "[Error]", "[Info]", "[Log]" };

	if (!msg.empty() && msg[msg.length() - 1] == '\n')
		msg.erase(msg.length() - 1);
	if (this->_fs.is_open())
		this->_fs << now << msgType[type] << ' ' << msg << std::endl;
}

std::string	Tintin_reporter::formatTimeNow( void ) const
{
	std::time_t	t = std::time(NULL);
	char		buffer[512];

	std::strftime(buffer, sizeof(buffer), "[%d/%m/%Y - %H:%M:%S]", std::localtime(&t));
	return (std::string(buffer));
}
