#include <iostream>
#include <math.h>
#include <fstream>
#include <vector>
#include <array>
#include <time.h>

#include <boost/format.hpp>
#include <boost/optional.hpp>

#define DEBUGMODE_FLAG 0

enum enum_elements
{
	E_ELEM_SHOP,
	E_ELEM_ENTERTAINMENT,
	E_ELEM_FOOD,
	E_MAX,
};

//< shpp, station, entertainment, .... >
//属性ごとの訪問回数(ただし全体の言った数で割った値 = 割合
//全体の分母から日常的に行っている場所の数は減らす
//< 

//R k,l (k,lユーザ) = K * L / ( | K | * | L | )
// = Σi ( K i * L i ) ^ 2 / ( ( sqrt( Σi( K i ) ^ 2 ) * ( sqrt( Σi( L i ) ^ 3 ) ) 
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
void place()
{

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