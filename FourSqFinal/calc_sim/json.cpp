#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <boost/optional.hpp>
#include <boost/progress.hpp>
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
			std::string name_;
			int day_;
			int time_;
			int time_count_;
			int elem_;
			std::string elem_name_;
			double latitude_, longitude_;

			data() : time_count_( 0 ){}

		};

		struct day
		{
			int day_;
			std::map< std::string, data > data_;
		};
		std::string name_;

		std::vector< day > day_;
		std::map< std::string, int > elem_count_;
		std::vector< std::string > base_state_;//地元
	};

	int get_element()
	{
		std::map< std::string, int > element_dectionary;
		return 0;
	}

	std::vector< std::pair< std::string, int > > map_to_array( const std::map< std::string, int > & map )
	{
		std::vector< std::pair< std::string, int > > result;

		for( auto it = map.begin(); it != map.end(); ++it )
			result.push_back( std::make_pair( it->first, it->second ) );

		const auto best_n = ( std::min< int > )( 10, map.size() );

		std::partial_sort( result.begin(), result.begin() + best_n, result.end(),
			[]( const std::pair< std::string, int > & a, const std::pair< std::string, int > & b )
		{
			return b.second > a.second;
		} );

		//ベストNを取り出すにはpartial_sortのほうが適当
		/*
		std::sort( result.begin(), result.end(), []( const std::pair< std::string, int > & a, const std::pair< std::string, int > & b )
		{
			return b.second > a.second;
		} );
		*/

		return std::move( result );
	}


	double calc( const std::vector< personal > & people, const personal & person )
	{
		const personal * best = nullptr;
		double max = -1.0;

		const auto person_ranking = map_to_array( person.elem_count_ );

		for( auto it = person.day_.begin(); it != person.day_.end(); ++it )
		{

			for( auto it2 = it->data_.begin(); it2 != it->data_.end(); ++it2 )
			{
				for( auto people_it = people.begin(); people_it != people.end(); ++people_it )
				{

					double sum = 0;

					const auto tmp_ranking = map_to_array( people_it->elem_count_ );
					const auto size = tmp_ranking.size();

					//自分は除く
					if( people_it->name_ == person.name_ )
						continue;

					for( auto pday = people_it->day_.begin(); pday != people_it->day_.end(); ++pday )
					{
						const auto f = pday->data_.find( it2->first );

						if( f != pday->data_.end() )
						{
							//お互いによく良く場所であれば、係数に加算
							double alpha = 1.0;
							double beta = 1.0;

							for( unsigned int i = 0; i < size; ++i )
							{
								if( tmp_ranking[ i ].first == it2->first )
								{
									alpha += 0.1 * ( size - i );
									break;
								}
							}

							const auto rsize = person_ranking.size();
							for( unsigned int i = 0; i < rsize; ++i )
							{
								if( person_ranking[ i ].first == it2->first )
								{
									beta += 0.1 * ( size - i );
									break;
								}
							}
							sum +=  0.000001 * it2->second.time_count_ * f->second.time_count_ * alpha * beta;
						}
					}
					if( max < sum )
					{
						max = sum;
						best = & ( * people_it );
					}
				}
			}
		}

		std::cout << "SIMILAR USER : " << best->name_ << std::endl;
		std::cout << "PARAM : " << max << std::endl;

		return max;
	}


	void show_all( const json_reader & jr )
	{
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
							int prev_time = -1;
							for( auto it3 = log->begin(), end3 = log->end(); it3 != end3; ++it3 )
							{
								personal::data data;

								if( const auto time = it3->second.get_optional< std::string >( "time" ) )
								{
									data.time_ = 60 * 60 * boost::lexical_cast< int >( time.get().substr( 0, 2 ) )\
										+ 60 * boost::lexical_cast< int >( time.get().substr( 3, 2 ) ) \
										+      boost::lexical_cast< int >( time.get().substr( 6, 2 ) );

									if( prev_time != -1 )
										data.time_count_ = prev_time - data.time_;
									else
										data.time_count_ = 0;

									prev_time = data.time_;
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
											data.name_ = name.get();
										}

										if( const auto elem = it4->second.get_optional< std::string >( "venue.location.name" ) )
										{
											data.elem_ = 0;//elem.get();
											data.elem_name_ = elem.get();
											++person.elem_count_[ elem.get() ];
										}

										if( const auto lat = it4->second.get_optional< double >( "venue.location.lat" ) )
										{
											data.longitude_ = lat.get();
										}

										if( const auto lng = it4->second.get_optional< double >( "venue.location.lng" ) )
										{
											data.latitude_ = lng.get();
										}
									}
								}

								d.data_[ data.city_name_ ] = data;
							}
						}

						person.day_.push_back( std::move( d ) );
					}
				}
				people.push_back( std::move( person ) );
			}

		}

		calc( people, people[ 1 ] );
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

UTF-8 BOM無し
*/

int main()
{
	json::json_reader js;
	{
	boost::progress_timer t;
	js.read_file( "output.txt" );
	}
	{
		boost::progress_timer t;
		show_all( js );
	}
	return 0;
}

