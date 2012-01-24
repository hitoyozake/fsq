#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <boost/optional.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

struct value
{
	std::string name_;
	double value_;

	value( const std::string & name, const double value ) : 
	name_( name ), value_( value ){}

	value(){}
};

value parse( const std::string & str )
{
	using namespace std;
	
	const auto f = str.find( "," );
	const auto name = str.substr( 0, f );

	const auto v = str.substr( f + 1, str.size() - f - 1 );

	return value( name, boost::lexical_cast< double >( v ) );
}

std::vector< value > read_file()
{
	using namespace std;

	string filename;

	cin >> filename;

	ifstream ifs( filename );
	vector< value > values;

	if( !ifs.fail() )
	{
		string str;

		while( ! ifs.eof() )
		{
			string input;
			getline( ifs, input );

			boost::algorithm::replace_all( input, " ", "" );
			
			if( input.find( "," ) != string::npos )
				values.push_back( parse( input ) );
		}
	}
	else
		cerr << "load failed" << endl;

	return std::move( values );
}

int main()
{
	using namespace std;
	const auto value1 = read_file();
	const auto value2 = read_file();

	map< string, double > valuemap1;

	for( auto it = value1.begin(); it != value1.end(); ++it )
	{
		valuemap1[ it->name_ ] = 0;
	}
	for( auto it = value2.begin(); it != value2.end(); ++it )
	{
		valuemap1[ it->name_ ] = 0;
	}

	auto valuemap2 = valuemap1;

	for( auto it = value1.begin(); it != value1.end(); ++it )
	{
		valuemap1[ it->name_ ] = it->value_;
	}
	for( auto it = value2.begin(); it != value2.end(); ++it )
	{
		valuemap2[ it->name_ ] = it->value_;
	}

	ofstream ofs( "output.csv" );

	ofs << "=================kyoto===================" << endl;

	for( auto it = valuemap1.begin(); it != valuemap1.end(); ++it )
		ofs << it->first << "," << it->second << endl;
	ofs << endl << endl;
	
	ofs << "=================notkyoto===================" << endl;

	for( auto it = valuemap2.begin(); it != valuemap2.end(); ++it )
		ofs << it->first << "," << it->second << endl;

	return 0;
}