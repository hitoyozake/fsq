#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <numeric>

#include <boost/optional.hpp>
#include <boost/progress.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include <boost/io/ios_state.hpp>


#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/regex.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>


#pragma comment( lib, "boost_system-vc100-mt-gd-1_47.lib" )


#include "distance.h"

#define NOMINMAX

namespace json
{
	using namespace boost::property_tree;
	class json_reader
	{
	public:
		bool read_file( const std::string & filename );
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
		std::map< std::string, std::string > venue_elem_;
		std::map< std::pair< std::string, std::string >, int > venue_vector_;
		//場所に対するvector(最大の時間が入る)
		std::map< std::string, bool > local_;

		struct day
		{
			int day_;
			std::vector< data > data_;
		};
		std::string name_;
		std::vector< day > day_;
		std::map< std::string, int > elem_count_;
		std::set< std::string > base_state_;//地元
	};

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

#pragma region 地元のスポットであるかどうか
	bool is_local( const personal & person, const std::string & state )
	{
		if( std::find( person.base_state_.begin(), person.base_state_.end(), state ) \
			!= person.base_state_.end() )
			return true;
		else
			return false;
	}
#pragma endregion

	double calc_p2p( const personal & person1, const personal & person2 )
	{
		double sum = 0;
		const auto p1_ranking = map_to_array( person1.elem_count_ );
		const auto p2_ranking = map_to_array( person2.elem_count_ );

		const auto p1_rank_size = p1_ranking.size();
		const auto p2_rank_size = p2_ranking.size();

		for( auto it = person1.venue_vector_.begin(); it != person1.venue_vector_.end(); ++it )
		{
			const auto f = person2.venue_vector_.find( it->first );

			if( f != person2.venue_vector_.end() )
			{
				//地元であれば計算しない
				if( is_local( person1, f->first.second ) )
				{
					//地元なので計算しない
					continue;
				}
				if( is_local( person2, f->first.second ) )
				{
					//地元なので計算しない
					continue;
				}

				double alpha = 1.0;
				double beta = 1.0;

				//よく行く場所かどうか
				for( unsigned int i = 0; i < p1_ranking.size(); ++i )
				{
					if( person1.venue_elem_.find( f->first.second ) != person1.venue_elem_.end() )
					{
						alpha += 1.0 - 0.1 * i; 
					}
				}
				for( unsigned int i = 0; i < p2_ranking.size(); ++i )
				{
					if( person2.venue_elem_.find( f->first.second ) != person2.venue_elem_.end() )
					{
						beta += 1.0 - 0.1 * i;
					}
				}
				sum += 0.000001 * alpha * it->second * beta * f->second;
			}
		}

		return sum;
	}
#pragma endregion

#pragma region 類似度計算
	double calc( const std::vector< personal > & people, const personal & person )
	{
		const auto person_ranking = map_to_array( person.elem_count_ );
		std::vector< std::pair< double, personal > > best_five;

		for( auto it = people.begin(); it != people.end(); ++it )
		{
			if( it->name_  == person.name_ )
				continue;
			
			const auto value = calc_p2p( person, *it );

			best_five.push_back( std::make_pair( value, * it ) );
		}


		const int five = std::min< int >( 5, best_five.size() );
		std::partial_sort( best_five.begin(), best_five.begin() + five, best_five.end(), \
			[]( const std::pair< double, personal > & a, const std::pair< double, personal  > & b )
		{
			return a.first > b.first;
		} );

		std::cout << boost::format( "user : %s, similar user : %s, value : %d\n\n" ) \
			% person.name_ % best_five[ 0 ].second.name_ % best_five[ 0 ].first;

		return best_five[ 0 ].first;
	}
#pragma endregion

#pragma region 地元で行く場所の属性統計 
	std::map< std::string, int > local_element( const personal & person )
	{
		std::map< std::string, int > elem_count;
		int count = 0;
		for( auto it = person.day_.begin(); it != person.day_.end(); ++it )
		{
			for( auto it2 = it->data_.begin(); it2 != it->data_.end(); ++it2 )
			{
				for( auto local_it = person.base_state_.begin(); local_it != person.base_state_.end(); ++local_it )
				{
					if( * local_it == it2->state_name_ )
					{
						++elem_count[ it2->elem_name_ ];
						++count;
					}
				}
			}
		}

		const auto size = std::max< int >( count, 1 );//elem_count.size();

		std::cout << person.name_ << std::endl;
		for( auto it = elem_count.begin(); it != elem_count.end(); ++it )
		{
			std::cout << boost::format( "%s : %0.3lf\n" ) % it->first % ( static_cast< double >( it->second ) / size );
		}

		return std::move( elem_count );
	}
#pragma endregion

#pragma region 京都で行く場所の属性統計 
	std::map< std::string, int > kyoto_element( const personal & person, const std::string & kyoto  )
	{
		std::map< std::string, int > elem_count;
		int count = 0;

		bool f = false;

		for( auto it = person.base_state_.cbegin(); it != person.base_state_.cend(); ++it )
		{	
			if( * it == kyoto )
			{
				f = true;
			}
		}

		if( !f )
			return std::move( elem_count );

		for( auto it = person.day_.begin(); it != person.day_.end(); ++it )
		{
			for( auto it2 = it->data_.begin(); it2 != it->data_.end(); ++it2 )
			{
				if( it2->state_name_  == kyoto )
				{
					++elem_count[ it2->elem_name_ ];
					++count;
				}
			}
		}

		const auto size = std::max< int >( count, 1 );//elem_count.size();

		std::cout << person.name_ << std::endl;
		for( auto it = elem_count.begin(); it != elem_count.end(); ++it )
		{
			//std::cout << boost::format( "%s : %0.3lf\n" ) % it->first % ( static_cast< double >( it->second ) / size );
		}

		return std::move( elem_count );
	}
#pragma endregion

#pragma region 京都で行く場所の属性統計 (地元じゃない人)
	std::map< std::string, int > kyoto_not_element( const personal & person, const std::string & kyoto  )
	{
		std::map< std::string, int > elem_count;
		int count = 0;

		for( auto local_it = person.base_state_.begin(); local_it != person.base_state_.end(); ++local_it )
		{
			if( * local_it == kyoto )
			{
				return std::move( elem_count );
			}
		}

		for( auto it = person.day_.begin(); it != person.day_.end(); ++it )
		{
			for( auto it2 = it->data_.begin(); it2 != it->data_.end(); ++it2 )
			{
				if( it2->state_name_ == kyoto )
				{
					++elem_count[ it2->elem_name_ ];
					++count;
				}
			}
		}

		const auto size = std::max< int >( count, 1 );//elem_count.size();

		//std::cout << person.name_ << std::endl;
		for( auto it = elem_count.begin(); it != elem_count.end(); ++it )
		{
			//std::cout << boost::format( "\t%s : %0.3lf\n" ) % it->first % ( static_cast< double >( it->second ) / size );
		}

		return std::move( elem_count );
	}
#pragma endregion

#pragma region パースkyoto
	std::vector< personal > create_profiles_kyoto( const json_reader & jr, const std::string & kt )
	{
		auto pt = jr.pt();

		std::vector< personal > people;

		if( const auto data = pt.get_child_optional( "data" ) )
		{
			for( auto it = data->begin(), end = data->end(); it != end; ++it )
			{
				personal person;
				bool kyoto = false;

				if( auto user = it->second.get_optional< std::string >( "user" ) )
				{
					person.name_ = std::move( user.get() );
				}

				int month = 0;
				typedef std::string state_name;
				std::map< int, std::set< state_name > > states;
				if( const auto days = it->second.get_child_optional( "days" ) )
				{
					for( auto it2 = days->begin(), end2 = days->end(); it2 != end2; ++it2 )
					{
						personal::day d;
						int d_num = 1;

						if( const auto day = it2->second.get_optional< std::string >( "day" ) )
						{
							try
							{
								d.day_ = boost::lexical_cast< int >( day.get().substr( 1 ) );
							}
							catch( std::exception & e )
							{
								d.day_ = 0;
							}
							const auto m = d.day_ / 100 - 100 * ( d.day_ / 10000 );
							d_num = d.day_ -  100 * ( d.day_ / 100 );

							if( m != month )
							{
								std::map< std::string, int > state_counts;

								//統計処理 & 地元登録
								for( auto sit = states.begin(); sit != states.end(); ++sit )
								{
									for( auto sit2 = sit->second.begin(); sit2 != sit->second.end(); ++sit2 )
									{
										//日にちごとに1回しか数えられない都道府県をカウントしていく
										++state_counts[ * sit2 ];
									}
								}

								for( auto scit = state_counts.begin(); scit != state_counts.end(); ++scit )
								{
									const auto order = 0.27; //6日程度

									//地元に登録
									if( static_cast< double >( scit->second ) / 31.0 > order )
									{
										person.base_state_.insert( scit->first );
									}
								}
								month = m;
								states.clear();
							}

						}

						if( const auto log = it2->second.get_child_optional( "log" ) )
						{
							int prev_time = -1;
							double prev_lat = -1;
							double prev_lon = -1;

							for( auto it3 = log->begin(), end3 = log->end(); it3 != end3; ++it3 )
							{
								personal::data data;

								if( const auto info = it3->second.get_child_optional( "info" ) )
								{
									for( auto it4 = info->begin(), end4 = info->end(); it4 != end4; ++it4 )
									{
										if( const auto city = it4->second.get_optional< std::string >( "venue.location.city" ) )
										{
											data.city_name_ = city.get();											
										}

										if( const auto state = it4->second.get_optional< std::string >( "venue.location.state" ) )
										{
											states[ d_num ].insert( state.get() );
											data.state_name_ = state.get();
											if( data.state_name_ == kt )
											{
												kyoto = true;
											}
										}

										if( const auto name = it4->second.get_optional< std::string >( "venue.name" ) )
										{
											data.name_ = name.get();
										}

										if( const auto categories = it4->second.get_child_optional( "venue.categories" ) )
										{
											for( auto cit = categories->begin(); cit != categories->end(); ++cit )
											{
												if( const auto elem = cit->second.get_optional< std::string >( "name" ) )
												{
													data.elem_ = 0;//elem.get();
													data.elem_name_ = elem.get();
													++person.elem_count_[ elem.get() ];
													person.venue_elem_[ data.name_ ] = elem.get();
												}
											}
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

								if( const auto time = it3->second.get_optional< std::string >( "time" ) )
								{
									try
									{
									data.time_ = 60 * 60 * boost::lexical_cast< int >( time.get().substr( 0, 2 ) )\
										+ 60 * boost::lexical_cast< int >( time.get().substr( 3, 2 ) ) \
										+      boost::lexical_cast< int >( time.get().substr( 6, 2 ) );
									}
									catch( std::exception & e )
									{
										data.time_ = 0;
									}
									//差を求める
									if( prev_time != -1 )
									{
										const auto dist = distance::get_distance \
											( prev_lat, prev_lon, data.latitude_, data.longitude_ );
										data.time_count_ = ( std::max )( 1, prev_time - data.time_ - static_cast< int >( dist ) );
									}
									else
									{
										data.time_count_ = 0;
									}
									//データ更新
									prev_time = data.time_;
									prev_lat = data.latitude_;
									prev_lon = data.longitude_;
								}

								d.data_.push_back( data );
								person.venue_vector_[ std::make_pair( data.name_, data.state_name_ ) ] = std::max< int >( person.venue_vector_[ std::make_pair( data.name_, data.state_name_ ) ], data.time_ );
							}
						}
						person.day_.push_back( std::move( d ) );
					}
				}
				
				if( kyoto )
				{
					//std::cout << person.name_ << std::endl;
					people.push_back( std::move( person ) );
				}
				kyoto = false;
			}

		}
		return std::move( people );
	}
#pragma endregion

#pragma region パース
	std::vector< personal > create_profiles( const json_reader & jr )
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

				int month = 0;
				typedef std::string state_name;
				std::map< int, std::set< state_name > > states;
				if( const auto days = it->second.get_child_optional( "days" ) )
				{
					for( auto it2 = days->begin(), end2 = days->end(); it2 != end2; ++it2 )
					{
						personal::day d;
						int d_num = 1;

						if( const auto day = it2->second.get_optional< std::string >( "day" ) )
						{
							d.day_ = boost::lexical_cast< int >( day.get().substr( 1 ) );

							const auto m = d.day_ / 100 - 100 * ( d.day_ / 10000 );
							d_num = d.day_ -  100 * ( d.day_ / 100 );

							if( m != month )
							{
								std::map< std::string, int > state_counts;

								//統計処理 & 地元登録
								for( auto sit = states.begin(); sit != states.end(); ++sit )
								{
									for( auto sit2 = sit->second.begin(); sit2 != sit->second.end(); ++sit2 )
									{
										//日にちごとに1回しか数えられない都道府県をカウントしていく
										++state_counts[ * sit2 ];
									}
								}

								for( auto scit = state_counts.begin(); scit != state_counts.end(); ++scit )
								{
									const auto order = 0.32; //10日程度

									//地元に登録
									if( static_cast< double >( scit->second ) / 31.0 > order )
									{
										person.base_state_.insert( scit->first );
									}
								}
								month = m;
								states.clear();
							}

						}

						if( const auto log = it2->second.get_child_optional( "log" ) )
						{
							int prev_time = -1;
							double prev_lat = -1;
							double prev_lon = -1;

							for( auto it3 = log->begin(), end3 = log->end(); it3 != end3; ++it3 )
							{
								personal::data data;

								if( const auto info = it3->second.get_child_optional( "info" ) )
								{
									for( auto it4 = info->begin(), end4 = info->end(); it4 != end4; ++it4 )
									{
										if( const auto city = it4->second.get_optional< std::string >( "venue.location.city" ) )
										{
											data.city_name_ = city.get();											
										}

										if( const auto state = it4->second.get_optional< std::string >( "venue.location.state" ) )
										{
											states[ d_num ].insert( state.get() );
											data.state_name_ = state.get();
										}

										if( const auto name = it4->second.get_optional< std::string >( "venue.name" ) )
										{
											data.name_ = name.get();
										}


										if( const auto categories = it4->second.get_child_optional( "venue.categories" ) )
										{
											for( auto cit = categories->begin(); cit != categories->end(); ++cit )
											{
												if( const auto elem = cit->second.get_optional< std::string >( "name" ) )
												{
													data.elem_ = 0;//elem.get();
													data.elem_name_ = elem.get();
													++person.elem_count_[ elem.get() ];
													person.venue_elem_[ data.name_ ] = elem.get();
												}
											}
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

								if( const auto time = it3->second.get_optional< std::string >( "time" ) )
								{
									data.time_ = 60 * 60 * boost::lexical_cast< int >( time.get().substr( 0, 2 ) )\
										+ 60 * boost::lexical_cast< int >( time.get().substr( 3, 2 ) ) \
										+      boost::lexical_cast< int >( time.get().substr( 6, 2 ) );

									//差を求める
									if( prev_time != -1 )
									{
										const auto dist = distance::get_distance \
											( prev_lat, prev_lon, data.latitude_, data.longitude_ );
										data.time_count_ = ( std::max )( 1, prev_time - data.time_ - static_cast< int >( dist ) );
									}
									else
									{
										data.time_count_ = 0;
									}
									//データ更新
									prev_time = data.time_;
									prev_lat = data.latitude_;
									prev_lon = data.longitude_;
								}

								d.data_.push_back( data );
								person.venue_vector_[ std::make_pair( data.name_, data.state_name_ ) ] = std::max< int >( person.venue_vector_[ std::make_pair( data.name_, data.state_name_ ) ], data.time_ );
							}
						}
						person.day_.push_back( std::move( d ) );
					}
				}
				std::cout << person.name_ << std::endl;
				for( auto it = person.base_state_.begin(); it != person.base_state_.end(); ++it )
					std::cout << * it << std::endl;
				people.push_back( std::move( person ) );
			}

		}
		return std::move( people );
	}
#pragma endregion

	bool json_reader::read_file( const std::string & filename )
	{
		ptree pt;
		try
		{
			json_parser::read_json( filename, pt );
		}
		catch( std::exception & e )
		{
			std::cerr << filename << std::endl;
			std::cerr << e.what() << std::endl;
			return false;
		}
		pt_ = std::move( pt );
		return true;
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

void thread_work( bool & lockf, bool & lockp, std::vector< std::string > & files, std::vector< json::personal > & profiles, const std::string & kt )
{
	using namespace std;
	while( files.size() )
	{
		string filename;

		if( ! lockf )
		{
			lockf = true;
			boost::this_thread::sleep( boost::posix_time::milliseconds( 10 ) );
			if( files.size() > 0 )
			{
				filename = files.back();
				files.pop_back();
				lockf = false;
				json::json_reader js;

				std::cerr << "start:" << filename << std::endl;

				if( js.read_file( filename ) )
				{
					const auto tmp = create_profiles_kyoto( js, kt );
					bool finished = false;
					do
					{
						if( lockp )
						{
							boost::this_thread::sleep( boost::posix_time::milliseconds( 20 ) );
						}
						else
						{
							lockp = true;
							boost::this_thread::sleep( boost::posix_time::milliseconds( 10 ) );
							for( auto it = tmp.begin(); it != tmp.end(); ++it )
								profiles.push_back( * it );
							lockp = false;
							finished = true;
						}
					}while( ! finished );
				}
			}
			else
				lockf = false;
		}
	}
}


void kyoto_all()
{
	using namespace std;
	using namespace json;

	json_reader js;
	namespace fs = boost::filesystem;

	vector< personal > profiles;
	vector< string > files;

	string kt;
	{
		ifstream ifs( "kyoto.txt" );
		getline( ifs, kt );
	}

	for( fs::directory_iterator it( fs::current_path() ), end; it != end; ++it )
	{
		boost::regex rgx( "([1-9]*|[a-z]*|[A-Z]*).txt" );
		
		boost::smatch match;
		const std::string s = it->path().filename().string();

		if( boost::regex_search( s, match, rgx ) )
		{
			std::cerr << "reading: " << s << std::endl;
			const std::string filename = s;

			files.push_back( std::move( filename ) );
		}
	}

	bool lockf = false;
	bool lockp = false;
	
	boost::thread th1( thread_work, std::ref( lockf ), std::ref( lockp ), std::ref( files ), std::ref( profiles ), kt );
	boost::thread th2( thread_work, std::ref( lockf ), std::ref( lockp ), std::ref( files ), std::ref( profiles ), kt );
	boost::thread th3( thread_work, std::ref( lockf ), std::ref( lockp ), std::ref( files ), std::ref( profiles ), kt );
	boost::thread th4( thread_work, std::ref( lockf ), std::ref( lockp ), std::ref( files ), std::ref( profiles ), kt );

	//終了待ち
	th1.join();
	th2.join();
	th3.join();
	th4.join();

	//ファイルの読み込みがすべて終わり
	
	map< string, int > kyoto;
	map< string, int > not_kyoto;
	int kcount = 0;
	int nkcount = 0;

	//==================================================
	//最後のbaseを使った、各ユーザごとの属性情報抽出用
	vector< map< string, int > > kusers;

	//==================================================

	//京都のみなので
	for( auto it = profiles.begin(); it != profiles.end(); ++it )
	{
		const auto k = json::kyoto_element( * it, kt );

		//地元が
		//京都だった場合の、京都での統計
		//京都じゃなかった場合の、京都での統計

		//json::calc( profiles, * it );
		for( auto it2 = k.cbegin(); it2 != k.cend(); ++it2 )
		{
			kyoto[ it2->first ] += it2->second;
			kcount += it2->second;
		}

		kusers.push_back( move( k ) );
	}

	cout << "=========KYOTO================" << endl;

	vector< pair< string, int > > ktv;

	for( auto it = kyoto.begin(); it != kyoto.end(); ++it )
	{
		ktv.push_back( make_pair( it->first, it->second ) );
	}
	sort( ktv.begin(), ktv.end(), []( const pair< string, int > & a, const pair< string, int > & b )
	{
		return a.second > b.second;
	} );

	const auto ktchecksum = accumulate( ktv.begin(), ktv.end(), 0, []( const int v, const pair< string, int > & p )
	{
		return v + p.second;
	} );
	

	cout << "CHECKIN NUM," << ktchecksum << endl;

	for( auto it = ktv.rbegin(); it != ktv.rend(); ++it )
		cout << it->first << " : " << 1.0 * it->second / kcount << endl;

	//==================================================
	//最後のbaseを使った、各ユーザごとの属性情報抽出用
	vector< map< string, int > > nkusers;

	//==================================================
	
	//京都のみなので、地元京都じゃない人の京都
	for( auto it = profiles.begin(); it != profiles.end(); ++it )
	{
		const auto nk = json::kyoto_not_element( * it, kt );
		//地元が
		//京都だった場合の、京都での統計
		//京都じゃなかった場合の、京都での統計

		//json::calc( profiles, * it );

		for( auto it2 = nk.cbegin(); it2 != nk.cend(); ++it2 )
		{
			not_kyoto[ it2->first ] += it2->second;
			nkcount += it2->second;
		}
		nkusers.push_back( move( nk ) );
	}

	cout << endl << endl;
	cout << "=============NOT KYOTO=======================" << endl;
	vector< pair< string, int > > nktv;
	for( auto it = not_kyoto.cbegin(); it != not_kyoto.cend(); ++it )
	{
		nktv.push_back( make_pair( it->first, it->second ) );
	}

	sort( nktv.begin(), nktv.end(), []( const pair< string, int > & a, const pair< string, int > & b )
	{
		return a.second > b.second;
	} );

	const auto nktchecksum = accumulate( nktv.begin(), nktv.end(), 0, []( const int v, const pair< string, int > & p )
	{
		return v + p.second;
	} );
	
	cout << "CHECKIN NUM," << nktchecksum << endl;

	for( auto it = nktv.crbegin(); it != nktv.crend(); ++it )
		cout << it->first << " : " << 1.0 * it->second / nkcount << endl;
	
	//===================================================
	//csv用に属性の並び・個数を全て統一したベースmapを作る
	set< string > base;

	for( auto it = not_kyoto.cbegin(); it != not_kyoto.cend(); ++it )
		base.insert( it->first );

	for( auto it = kyoto.cbegin(); it != kyoto.cend(); ++it )
		base.insert( it->first );

	//===========================================
	//出力
	string out;
	for( auto it = base.cbegin(); it != base.cend(); ++it )
	{
		out += * it;
		out += ",";
		for( auto it2 = kusers.begin(); it2 != kusers.end(); ++it2 )
		{
			out += boost::lexical_cast< string >( static_cast< double >( ( * it2 )[ * it ] ) );
			out += ",";
		}
		out.pop_back();
		out += "\n";
	}

	cout << out << endl;
	out.clear();

	for( auto it = base.cbegin(); it != base.cend(); ++it )
	{
		out += * it;
		out += ",";
		for( auto it2 = nkusers.begin(); it2 != nkusers.end(); ++it2 )
		{
			out += boost::lexical_cast< string >( static_cast< double >( ( * it2 )[ * it ] ) );
			out += ",";
		}
		out.pop_back();
		out += "\n";
	}
	
	cout << out << endl;
	//===================================================
	cout << endl << endl;
	cout << "=====kyoto=====" << endl;
	
	for( auto it = base.cbegin(); it != base.cend(); ++it )
	{
		cout << * it << "," << 1.0 * kyoto[ * it ] / kcount << endl;
	}

	cout << endl << endl;
	cout << "=====notkyoto=====" << endl;
	
	for( auto it = base.cbegin(); it != base.cend(); ++it )
	{
		cout << * it << "," << 1.0 * not_kyoto[ * it ] / nkcount << endl;
	}

	//実際この比率をかけて、分類できるかどうか
	//people-> kyotoのスポット -> [属性] * 率 のsum
	// local > leader なら地元

	cout << endl << endl << endl;
	cout << "==================local check===================" << endl;
	{
		map< string, int > local_users;
		map< string, int > not_local_users;

		int local = 0;
		int not_local = 0;

		int local_elem_count = 0;
		int not_local_elem_count = 0;


		//それで振り分けたユーザの属性情報比較・・・・有意差なら、地元との差別化完了
		for( auto it = profiles.begin(); it != profiles.end(); ++it )
		{
			it->base_state_.clear();//地元情報は削除
			const auto nk = json::kyoto_not_element( * it, kt );

			double val1 = 0.0;
			double val2 = 0.0;
			for( auto it2 = nk.begin(); it2 != nk.end(); ++it2 )
			{
				val1 += 1.0 * kyoto[ it2->first ] * it2->second / kcount;
				val2 += 1.0 * not_kyoto[ it2->first ] * it2->second / nkcount;
			}

			if( val1 > val2 )
			{
				++local;
				for( auto it2 = nk.begin(); it2 != nk.end(); ++it2 )
				{
					local_elem_count += it2->second;
					local_users[ it2->first ] += it2->second;
				}
				//地元
			}
			else
			{
				++not_local;
				//観光客
				for( auto it2 = nk.begin(); it2 != nk.end(); ++it2 )
				{
					not_local_elem_count += it2->second;
					not_local_users[ it2->first ] += it2->second;
				}
			}
		}

		cout << "==================LOCAL======================" << endl;

		for( auto it = base.cbegin(); it != base.cend(); ++it )
		{
			cout << * it << "," << 1.0 * local_users[ * it ] / local_elem_count << endl;
		}

		cout << endl << endl << endl;

		cout << "==================KKKK======================" << endl;


		for( auto it = base.cbegin(); it != base.cend(); ++it )
		{
			cout << * it << "," << 1.0 * not_local_users[ * it ] / not_local_elem_count << endl;
		}
	}
}



int main()
{

	std::ofstream ofs( "profilelog.txt" );
	std::cout.rdbuf( ofs.rdbuf() );

	kyoto_all();

	//json::json_reader js;
	//{
	//	boost::progress_timer t;
	//	js.read_file( "output.txt" );
	//}
	//{
	//	boost::io::ios_rdbuf_saver( std::ref( std::cout ) );
	//	boost::progress_timer t;
	//	const auto profiles = create_profiles( js );

	//	/*for( int i = 0; i < 2; ++i )
	//	{
	//		json::calc( profiles, profiles[ i ] );
	//	}*/
	//	for( auto it = profiles.begin(); it != profiles.end(); ++it )
	//	{
	//		json::local_element( * it );

	//		//json::calc( profiles, * it );
	//	}

	//}
	return 0;
}

