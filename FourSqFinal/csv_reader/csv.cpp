#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>

struct value
{
	std::string name_;
	double value_;

	value( const std::string & name, const double value ) : 
	name_( name ), value_( value ){}

	value(){}
};

boost::optional< std::vector< value > > parse( const std::string & str )
{
	using namespace std;
	
	const auto f = str.find( "," );
	const auto name = str.substr( 0, f );
	const auto v = str.substr( f + 1, str.size() - f - 1 );

	cout << name << endl;
	cout << v << endl;

	vector< value > result;

	return boost::optional< vector< value > >( std::move( result ) );
}

int main()
{
	using namespace std;

	string filename;

	cin >> filename;

	ifstream ifs( filename );

	if( !ifs.fail() )
	{
		string str;

		while( ! ifs.eof() )
		{
			string input;
			getline( ifs, input );
			str += input;
			str += "\n";
		}

		//cout << str << endl;

		if( auto values = parse( str ) )
		{
			for( auto it = values->begin(); it != values->end(); ++it )
			{
				cout << it->name_ << ":" << it->value_ << endl;
			}
		}
	}
	else
		cerr << "load failed" << endl;

	return 0;
}