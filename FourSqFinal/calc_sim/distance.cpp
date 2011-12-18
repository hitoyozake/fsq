#include "distance.h"

namespace distance
{
	double get_distance( const double lat1, const double lon1,
		const double lat2, const double lon2 )
	{
		const double pi = 3.14159265358979;

		const double \
		from_x = lon1 * pi / 180,
		from_y = lat1 * pi / 180,
		to_x = lon2 * pi / 180,
		to_y = lat1 * pi / 180;

		const double deg = sin( from_y ) * sin( to_y ) \
			+ cos( from_y ) * cos( to_y ) * cos( to_x - from_x );

		const double distance = 6378140.0 * ( atan( -deg / sqrt( -deg * deg + 1 ) )\
			+ pi / 2.0 ) / 1000.0;

		return distance;
	}
}