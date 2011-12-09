#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <map>

#include <array>
#include <boost/lexical_cast.hpp>
#include <boost/progress.hpp>

namespace basic_string_hash
{
	class basic_string_hash
	{
	public:
		basic_string_hash & operator=( const basic_string_hash & bsh )
		{
			k_ = bsh.k_;
			return * this;
		}

		basic_string_hash & operator=( const std::string & str )
		{
			k_ = & ( * key_pool_.insert( std::string() ).first );
			return * this;
		}


		basic_string_hash()
			: k_ ( & ( * key_pool_.insert( std::string() ).first ) )

		{
		}

		basic_string_hash( const std::string & key )
			: k_ ( & ( * key_pool_.insert( key ).first ) )
		{
		}

		basic_string_hash( const basic_string_hash & bsh )
			: k_ (bsh.k_ )		
		{
		}

		std::size_t hash() const
		{
			return reinterpret_cast< std::size_t >( k_ );
		}

		struct equal_str_hash
		{
			bool operator ()( const basic_string_hash & a, const basic_string_hash & b ) const
			{
				return a.hash() == b.hash();
			}
		};

	private:
		const std::string * k_;
		static std::unordered_set< std::string > key_pool_;
	};

	std::unordered_set< std::string > basic_string_hash::key_pool_;

	struct myhasher
	{
		std::size_t operator() ( const basic_string_hash & bsh ) const
		{
			return static_cast< std::size_t >( bsh.hash() );
		}
	};



	template< std::size_t SIZE >
	void init( std::array< std::string, SIZE > & str_table, std::array< basic_string_hash, SIZE > & table ) {
		std::string key;
		for ( int i = 0; i < SIZE; ++i )
		{
			const std::string buf = "string string 日本語:" + boost::lexical_cast< std::string >( i );  
			str_table[ i ] = buf;
			table[ i ] = basic_string_hash( buf );
		}
	}

	template<class Container, class T, std::size_t SIZE >
	void trial(Container & container, const T & table, int repeat )
	{

		boost::progress_timer t;
		// table[0..N-1]をcontainerに挿入する。
		for ( int i = 0; i < SIZE; ++i )
			container[ table[ i ] ] = boost::lexical_cast< std::string>( i );

		// table[0..N-1]をcontainerから検索し、その処理時間を求める。

		while ( repeat-- )
		{
			for ( int i = 0; i < SIZE; ++i )
			{
				container.find( table[ i ] );
			}
		}
	}


	void map_test()
	{
		std::unordered_map< basic_string_hash, std::string, myhasher, basic_string_hash::equal_str_hash > bshmap;
		std::map< std::string, std::string > strmap;

		const int size = 2500;
		std::array< std::string, size >  str_table;
		std::array< basic_string_hash, size >  bsh_table;

		init( str_table, bsh_table );

		trial< decltype( bshmap ), decltype( bsh_table ), size >( bshmap, bsh_table, 1 );
		trial< decltype( strmap ), decltype( str_table ), size >( strmap, str_table, 1 );
	}

	int main()
	{
		map_test();

		return 0;
	}
}