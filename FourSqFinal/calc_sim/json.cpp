#include <iostream>
#include <string>
#include <vector>
#include <map>

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


	void show_all( const json_reader & jr )
	{
		std::vector< std::pair< std::string, \
			std::vector< std::pair< std::string, std::vector< std::string > > > > > vector;

		auto pt = jr.pt();

		if( const auto data = pt.get_child_optional( "data" ) )
		{
			for( auto it = data->begin(), end = data->end(); it != end; ++it )
			{
				if( auto user = it->second.get_optional< std::string >( "user" ) )
				{
					std::cout << user.get() << std::endl;
				}

				if( const auto days = it->second.get_child_optional( "days" ) )
				{
					for( auto it2 = days->begin(), end2 = days->end(); it2 != end2; ++it2 )
					{
						if( const auto day = it2->second.get_optional< std::string >( "day" ) )
						{
							std::cout << "\tday : " << day.get() << std::endl;
						}

						if( const auto log = it2->second.get_child_optional( "log" ) )
						{
							for( auto it3 = log->begin(), end3 = log->end(); it3 != end3; ++it3 )
							{
								if( const auto time = it3->second.get_optional< std::string >( "time" ) )
								{
									std::cout << "\t\ttime : " << time.get() << std::endl;
								}
								if( const auto info = it3->second.get_child_optional( "info" ) )
								{
									for( auto it4 = info->begin(), end4 = info->end(); it4 != end4; ++it4 )
									{
										if( const auto city = it4->second.get_optional< std::string >( "venue.location.city" ) )
										{
											std::cout << "\t\t\tcity : " << city.get() << std::endl;
										}
									}
								}

							}
						}
					}
				}
			}

		}
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
				-day
				log.child
				{
					-time
					info.child
					{
						-venue.location.lat
						-venue.location.lng
						-venue.city
						-venue.state
						-venue.categories.name ( shortName, puralName )
					}
				}
			}
		}
	}
}

UTF-8 BOM–³‚µ
*/

int main()
{
	json::json_reader js;
	js.read_file( "outputs.txt" );
	show_all( js );

	return 0;
}

