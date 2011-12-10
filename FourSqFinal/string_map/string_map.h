#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace string_map
{
	namespace stx
	{
		template< class T >
		struct hash_maker
		{
			std::size_t operator() ( const T & a )const
			{
				return a.hash();
			}
		};


		template< class Key, class T >
		class basic_symbole
		{
		public:
			typedef std::size_t hash_type;
			typedef Key key_type;

			basic_symbol()
			{ k = &( * key_pool().insert( key_type()).first ); }
			basic_symbol(const key_type& key)
			{ k = &(*key_pool().insert(key).first); }
			basic_symbol(const basic_symbol& sym)
				: k(sym.k) {}

			basic_symbol& operator=(const basic_symbol& sym)
			{ k = sym.k; return *this; }
			basic_symbol& operator=(const key_type& key)
			{ k = &(*key_pool().insert(key).first); return *this; }

			const key_type& key() const  { return *k; }
			hash_type hash() const       { return hash_type(k); }

		private:
			typedef std::unordered_set< key_type > pool_allocator;
			static pool_allocator& key_pool();
			const key_type* k;
		};


	}
}


