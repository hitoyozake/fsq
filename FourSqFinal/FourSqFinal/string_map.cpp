#include "string_map.h"
#include <array>
#include <map>

#include <boost/lexical_cast.hpp>
#include <boost/progress.hpp>
#include <boost/functional/hash.hpp>

namespace map
{
	template< std::size_t SIZE >
	void init( std::array< std::string, SIZE > & str_table, std::array< stx::string_symbol, SIZE > & table ) {
		std::string key;
		for ( int i = 0; i < SIZE; ++i )
		{
			const std::string buf = "string string 日本語:" + boost::lexical_cast< std::string >( i );  
			str_table[ i ] = buf;
			table[ i ] = stx::string_symbol( buf );
		}
	}

	template<class Container, class T, std::size_t SIZE >
	void trial(Container & container, const T & table, int repeat ) {

		boost::progress_timer t;
		// table[0..N-1]をcontainerに挿入する。
		for ( int i = 0; i < SIZE; ++i )
			container[ table[ i ] ] = boost::lexical_cast< std::string>( i );

		// table[0..N-1]をcontainerから検索し、その処理時間を求める。

		while ( repeat-- )
		{
			for ( int i = 0; i < SIZE; ++i )
			{
				std::cout << container.find( table[ i ] ) << std::endl;
			}
		}
	}


	void test( std::unordered_map< stx::string_symbol, std::string > hoge )
	{
		//hoge[ (stx::string_symbol)"abc" ] = "cd";
		//hoge.find( (stx::string_symbol)"abc" );
	}



	void map_test()
	{
		std::unordered_map< stx::string_symbol, std::string, stx::hash< stx::string_symbol> > symmap;
		std::map< std::string, std::string > strmap;

		const int size = 25000;
		std::array< std::string, size >  str_table;
		std::array< stx::string_symbol, size >  stx_table;

		init( str_table, stx_table );

		trial< decltype( symmap ), decltype( stx_table ), size >( symmap, stx_table, 100 );
		trial< decltype( strmap ), decltype( str_table ), size >( strmap, str_table, 100 );
	}
}

int main()
{
	map::map_test();
	return 0;
}