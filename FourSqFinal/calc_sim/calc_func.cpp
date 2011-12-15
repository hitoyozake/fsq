#include <iostream>
#include <map>
#include <vector>

template< class Key >
double compair_all( const std::map< Key, double > & m1,
	const std::map< Key, double > & m2 )
{
	double sum = 0;
	for( auto it = m1.begin(), end = m1.end(); it != end; ++it )
	{
		sum += m1.at( it->first ) * m2.at( it->first );
	}
	return sum;
}

void test()
{
	compair_all( std::map< int, double >(), std::map< int, double >() );
}

