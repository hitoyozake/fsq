#include <iostream>
#include <string>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

int main()
{
	using namespace boost::property_tree;

	ptree pt;
	
	try
	{
		json_parser::read_json( "output2.txt", pt );
	}
	catch( std::exception & e )
	{
		std::cerr << e.what() << std::endl;
	}

	if( const auto result = pt.get_optional< std::string >( "venue.id" ) )
	{
		std::cout << result.get() << std::endl;
	}

	return 0;
}