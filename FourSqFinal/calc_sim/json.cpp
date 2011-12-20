#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

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

	struct personal
	{
		struct data
		{
			std::string city_name_;
			std::string state_name_;
			int day_;
			int time_;
			int elem_;
		};

		struct day
		{
			int day_;
			std::vector< data > data_;
		};
		std::string name_;

		std::vector< day > day_;
		std::map< std::string, int > elem_count_;
		std::vector< std::string > base_state_;//ínå≥
	};

	int get_element()
	{
		std::map< std::string, int > element_dectionary;
		return 0;
	}




	void show_all( const json_reader & jr )
	{
		std::vector< std::pair< std::string, \
			std::vector< std::pair< std::string, std::vector< std::string > > > > > vector;

		auto pt = jr.pt();

		std::vector< personal > people;

		if( const auto data = pt.get_child_optional( "data" ) )
		{
			for( auto it = data->begin(), end = data->end(); it != end; ++it )
			{
				personal person;

				if( auto user = it->second.get_optional< std::string >( "user" ) )
				{
					person.name_ = std::move( user.get() );
				}

				if( const auto days = it->second.get_child_optional( "days" ) )
				{
					for( auto it2 = days->begin(), end2 = days->end(); it2 != end2; ++it2 )
					{
						personal::day d;
						if( const auto day = it2->second.get_optional< std::string >( "day" ) )
						{
							d.day_ = boost::lexical_cast< int >( day.get().substr( 1 ) );
						}

						if( const auto log = it2->second.get_child_optional( "log" ) )
						{

							for( auto it3 = log->begin(), end3 = log->end(); it3 != end3; ++it3 )
							{
								personal::data data;
								
								if( const auto time = it3->second.get_optional< std::string >( "time" ) )
								{
									std::cout << "\t\ttime : " << time.get() << std::endl;
									data.time_ = 60 * 60 * boost::lexical_cast< int >( time.get().substr( 0, 2 ) )\
										+ 60 * boost::lexical_cast< int >( time.get().substr( 3, 2 ) ) \
										+      boost::lexical_cast< int >( time.get().substr( 6, 2 ) );
								}
								if( const auto info = it3->second.get_child_optional( "info" ) )
								{
									for( auto it4 = info->begin(), end4 = info->end(); it4 != end4; ++it4 )
									{
										if( const auto city = it4->second.get_optional< std::string >( "venue.location.city" ) )
										{
											data.city_name_ = city.get();											
										}

										if( const auto name = it4->second.get_optional< std::string >( "venue.name" ) )
										{
											data.elem_ = 0;//name.get();
										}

										if( const auto elem = it4->second.get_optional< std::string >( "venue.location.name" ) )
										{
											std::cout << "\t\t\telement : " << elem.get() << std::endl;
										}

										if( const auto lat = it4->second.get_optional< std::string >( "venue.location.lat" ) )
										{
											std::cout << "\t\t\t\tlatitude : " << lat.get() << std::endl;
										}

										if( const auto lng = it4->second.get_optional< std::string >( "venue.location.lng" ) )
										{
											std::cout << "\t\t\t\tlongitude : " << lng.get() << std::endl;
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

UTF-8 BOMñ≥Çµ
*/

int main()
{
	json::json_reader js;
	js.read_file( "outputs.txt" );
	show_all( js );

	return 0;
}

