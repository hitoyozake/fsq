#include <iostream>
#include <math.h>
#include <fstream>
#include <numeric>
#include <vector>
#include <array>
#include <map>
#include <time.h>

#include <boost/format.hpp>
#include <boost/optional.hpp>

#define DEBUGMODE_FLAG 0

#if 0

enum enum_elements
{
	E_ELEM_SHOP,
	E_ELEM_ENTERTAINMENT,
	E_ELEM_FOOD,
	E_MAX,
};

//その日のうちの最初と最後は知る必要があるので
//dayという概念は必要


#pragma region database
class database
{
public:
	typedef unsigned int day;
	typedef unsigned int time;
	typedef unsigned int state;

	std::string spot()
	{
		
	}

	struct spot
	{
		std::string name_;
		std::string category_;
		std::string city_;
		double latitude_;
		double longitude_;

		spot( const std::string & name, const std::string & category, const std::string & city, \
			const double latitude, const double longitude ) : \
			name_( name ), category_( category ), latitude_( latitude ), longitude_( longitude )
		{}

		spot(){}

	};
private:
	std::map< state, int > counter_;
	std::vector< spot * > all_;
	std::map< spot * const, int > all_map_;
	std::map< std::string, time > category_;
	std::map< std::string, day > history_;
	std::vector< std::string > black_list_;
	//追加されて無かったら、value = 0として返す
};
#pragma endregion

//同じstate/cityにおいて、何日間かけてつぶやいたか


struct profile
{
	std::map< std::string, database > city_history_;
	unsigned int id_;
};



//< shpp, station, entertainment, .... >
//属性ごとの訪問回数(ただし全体の言った数で割った値 = 割合
//全体の分母から日常的に行っている場所の数は減らす
//< 

//R k,l (k,lユーザ) = K * L / ( | K | * | L | )
// = Σi ( K i * L i ) ^ 2 / ( ( sqrt( Σi( K i ) ^ 2 ) * ( sqrt( Σi( L i ) ^ 2 ) ) 
//R = 相関係数
double calc( const std::vector< double > & k, const std::vector< double > & l )
{
	assert( k.size() == l.size() );

	double x = 0;
	double ky = 0;
	double ly = 0;

	for( std::size_t i = 0, size = k.size(); i < size; ++i )
	{
		x += k[ i ] * l[ i ];//pow( k[ i ] * l[ i ], 2 );
		ky += pow( k[ i ], 2 );
		ly += pow( l[ i ], 2 );
	}
	
	const double relation = x / ( sqrt( ky ) * sqrt( ly ) );

#if DEBUGMODE_FLAG
	std::cout << boost::format( "x : %0.3lf, a : %0.3lf, b : %0.3lf, R : %0.3lf\n" ) % x % ky % ly % relation;
#endif

	return relation;
}

//cityの記憶
//cityが同じ且つ複数日でまたいで記録していた場合は地元
//行動範囲に基づく類似度計算
boost::optional< std::vector< int > > get_time( const std::vector< int > & times )
{
	std::vector< int > result;

	if( times.size() <= 1 )
		return boost::optional< std::vector< int > >();

	//各観光地の、各属性(博物館他)の行動時間の平均をとっておくのもありかも
	//行動範囲が広い == 周る時間が短くて、多数の属性を得ている?

	//マルコフ連鎖的なものを求めていないので順番は見ない

	for( std::size_t i = 0, size = times.size(); i < size - 1; ++i )
	{
		result.push_back( times[ i ] - times[ i + 1 ] );
	}

	return boost::optional< std::vector< int > >( result );
}


int main()
{
	std::vector< double > user_a( 5 );
	std::vector< double > user_b( 5 );

	for( int i = 0; i < 5; ++i )
	{
		user_a[ i ] = rand() % 10;
		user_b[ i ] = rand() % 10;
	}

	std::cout << calc( user_a, user_b ) << std::endl;


	return 0;
}

#endif