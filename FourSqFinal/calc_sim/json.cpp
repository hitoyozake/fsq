#include <iostream>
#include <string>
#include <vector>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace json
{
	using namespace boost::property_tree;
	class json_reader
	{
	public:
		void read_file( const std::string & filename );
		ptree pt() const{ return pt_; }

	private:
		ptree pt_;
	};

	struct data
	{
		std::string city_name_;
		std::string state_name_;
		int day_;
		int time_;
	};

	void get_data( const json_reader & jr )
	{

	}

	void json_reader::read_file( const std::string & filename )
	{
		ptree pt;
		try
		{
			json_parser::read_json( filename, pt );
		}
		catch( std::exception & e )
		{
			std::cerr << e.what() << std::endl;
			return;
		}
		pt_ = std::move( pt );
	}
}

/*

data.child

	{
		-user
		days.child
			{
				
			}
		}
	}
}






*/

