#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include <boost/asio.hpp>
#include <boost/optional.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <boost/asio/ssl.hpp>

//icuuc
#include <Windows.h>

#pragma comment(lib, "libeay32MDd.lib")
#pragma comment(lib, "ssleay32MDd.lib")

namespace search
{
	boost::optional< std::string > search_ssl( const std::string & url )
	{
		try
		{
			const auto index = url.find( "/" );

			if( index == std::string::npos )
				return boost::optional< std::string >();

			const std::string uri = url.substr( 0, index );
			const std::string host = uri;
			const std::string site = url.substr( index );

			boost::asio::io_service io_service;

			boost::asio::ssl::context context( io_service, boost::asio::ssl::context::sslv3_client );
			boost::asio::ssl::stream< boost::asio::ip::tcp::socket > ssl_stream( io_service, context );

			//resolveを使ってサイトが存在するのか確認、そして接続
			ssl_stream.lowest_layer().connect( * boost::asio::ip::tcp::resolver( io_service ).resolve\
				( boost::asio::ip::tcp::resolver::query( uri, "https" ) ) );

			ssl_stream.handshake( boost::asio::ssl::stream_base::client );

			//requestを作成する-------------------------------
			boost::asio::streambuf request;
			std::ostream request_stream( & request );

			request_stream << "GET /" << site << " HTTP/1.0\r\n";
			request_stream << "Host: " << host << "\r\n";
			request_stream << "\r\n";
			//------------------------------------------------

			//リクエストの送信
			boost::asio::write( ssl_stream, request );

			//リクエストの受信と表示
			boost::asio::streambuf res;
			boost::system::error_code error;
			std::string log_str;
			while( boost::asio::read( ssl_stream, res, boost::asio::transfer_at_least(1), error ) )
			{
				std::ostringstream ss;
				//std::string str = boost::asio::buffer_cast< const char * >( res.data() ); 
				//std::cout << str;
				ss << & res;
				log_str += ss.str();

				if( log_str.find( "options['checkinId']" ) != std::string::npos )
					break;
			}

			const auto e_mes = error.message();

			const auto venue = log_str.find( "options['venue']" );
			const auto checkin = log_str.find( "options['checkinId']" );
			//std::cout << log_str;
			if(  venue != std::string::npos && checkin != std::string::npos )
			{
				auto tmp_str = log_str.substr( venue, checkin - venue - 1 );
				boost::algorithm::replace_all( tmp_str, "\n", "" );
				boost::algorithm::replace_all( tmp_str, "\t", "" );
				boost::algorithm::replace_all( tmp_str, " ", "" );
				boost::algorithm::replace_all( tmp_str, "\\", "" );//エスケープ文字削除


				std::cout << tmp_str << std::endl;
			}
			return boost::optional< std::string >();
		}
		catch( std::exception & e )
		{
			return boost::optional< std::string >();
		}
	}



	boost::optional< std::string > search_http( const std::string & httpurl )
	{
		try
		{
			using namespace boost::asio;
			// www.google.co.jp の http サービスに接続

			const std::string url = httpurl.substr( httpurl.find( "//" ) + 2 ); 

			const auto index = url.find( "/" );

			if( index == std::string::npos )
				return boost::optional< std::string >();

			const std::string uri = url.substr( 0, index );
			const std::string host = uri;
			const std::string site = url.substr( index );

			ip::tcp::iostream s( uri, "http" );

			// 送信
			s << "GET /" << site <<" HTTP/1.0\r\n";
			s << "Host: " << host << "\r\n";
			s << "\r\n";
			s << std::flush;  // バッファに溜めずに確実にネットワーク送信

			// 受信
			std::string line;
			std::string result;
			while( getline(s, line) )
			{
				result += line+ "\n";
				//std::cout << line << std::endl;
			}
			return boost::optional< std::string >( result );
		}
		catch( std::exception & e )
		{
			std::cerr << e.what() << std::endl;
			return boost::optional< std::string >();
		}
	}
}
int main()
{
	using namespace search;
	std::ifstream ifs( "input.txt" );
	std::ofstream ofs( "output2.txt" );

	std::cout.rdbuf( ofs.rdbuf() );

	while( ! ifs.eof() )
	{
		std::string str;
		std::getline( ifs, str );

		if( str.find( "http://" ) != std::string::npos )
		{
			if( str.find( "http://t.co" ) != std::string::npos )
			{
				const auto tco_url = str.substr( str.find( "http" ), str.find( "{" ) - str.find( "http" ) - 1 );
				const auto tco = search_http( tco_url );

				if( tco )
				{
					const auto http_index = tco->find( "http" );
					if( http_index == std::string::npos )
						continue;
					const auto url_4sq = tco->substr( http_index, tco->find( "Cache" ) - http_index - 2 );
					//4sq抽出 -2は\n切り

					const auto fsq = search_http( url_4sq );

					if( fsq )
					{
						const auto s = * fsq;
						//リダイレクト処理
						if( fsq->find( "href=\"https://foursquare" ) != std::string::npos )
						{
							const auto hrefsub = fsq->substr( fsq->find( "href=\"https://foursquare" ) );
							const auto href_index = fsq->find( "\">" );
							const auto len = hrefsub.find( "\">" );
							const auto url_fsq = hrefsub.substr( 14, len - 14 );
							search_ssl( url_fsq );
							//sh.search_ssl( url_fsq );
						}
					}
				}
			}
			if( str.find( "http://4sq" ) != std::string::npos )
			{
				const auto fsu = str.substr( str.find( "http" ) );
				const auto fsq_url = fsu.substr( 0, fsu.find( " " ) );
				const auto fsq = search_http( fsq_url );

				if( fsq )
				{
					const auto fsq_href_index = fsq->find( "href=" );
					//リダイレクト処理
					if( fsq->find( "href=" ) != std::string::npos )
					{
						const auto hrefsub = fsq->substr( fsq->find( "href=" ) );
						const auto href_index = fsq->find( "\">" );
						const auto len = hrefsub.find( "\">" );
						const auto url_fsq = hrefsub.substr( 14, len - 14 );
						search_ssl( url_fsq );
					}
				}
			}
		}
	}

	return 0;
}