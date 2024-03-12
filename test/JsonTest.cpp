#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>
#include <charconv>
#include <map>
#include <memory>
#include <string>
#include <variant>
#include <vector>

using namespace psm;

namespace json_test
{
	namespace peg
	{
		struct IntegerValue : Integer
		{
		};
		struct BooleanValue : One< Str< 't', 'r', 'u', 'e' >, Str< 'f', 'a', 'l', 's', 'e' > >
		{
		};
		struct StringValue : Seq< Char< '"' >, Star< One< Range< 'a', 'z' >, Range< 'A', 'Z' >, Range< '0', '9' >, Char< ' ' >, Char< '_' > > >, Char< '"' > >
		{
		};
		using Value = One< StringValue, IntegerValue, BooleanValue >;
		struct Name : Seq< Char< '"' >, Plus< One< Range< 'a', 'z' >, Range< 'A', 'Z' >, Range< '0', '9' >, Char< '_' > > >, Char< '"' > >
		{
		};
		using Space = Star< One< Char< ' ' >, Char< '\n' >, Char< '\t' > > >;
		using ArrayCore = Seq< Space, If< Char< '{' >, Ref< Tag0 >, If< Char< '[' >, Ref< Tag1 >, Value > > >;
		using Array = GenA< Tag1, 5, Seq< ArrayCore, Space, Star< Seq< Char< ',' >, ArrayCore, Space > >, Char< ']' > > >;
		using ObjectCore = Seq< Space, Name, Space, Char< ':' >, Space, If< Char< '{' >, Ref< Tag0 >, If< Char< '[' >, Array, Value > > >;
		using Object = GenA< Tag0, 5, Seq< ObjectCore, Space, Star< Seq< Char< ',' >, ObjectCore, Space > >, Char< '}' > > >;
		using Rule = Seq< Space, If< Char< '{' >, Object, If< Char< '[' >, Array > > >;
	} // namespace peg

	struct Object;
	struct Array;

	using ObjectPtr = std::unique_ptr< Object >;
	using ArrayPtr = std::unique_ptr< Array >;

	using Value = std::variant< long long, bool, std::string, ObjectPtr, ArrayPtr >;

	struct Object : std::map< std::string, Value >
	{
	};

	struct Array : std::vector< Value >
	{
	};

	struct Action
	{
		using Rules = std::tuple< peg::IntegerValue, peg::BooleanValue, peg::StringValue, peg::Name, Char< '{' >, Char< '[' >, Char< ']' >, Char< '}' > >;

		Value root;
		std::vector< std::variant< Object*, Array* > > stack;
		Value* pValue = nullptr;

		template< typename Rule >
		void operator()( const Rule& rule, std::size_t pos, const std::string_view& sv )
		{
			if constexpr( std::is_same_v< Rule, Char< '{' > > )
			{
				if( stack.empty() )
				{
					root = ObjectPtr( new Object );
					stack.emplace_back( std::get< ObjectPtr >( root ).get() );
				}
				else
				{
					auto newObj = ObjectPtr( new Object );
					stack.emplace_back( newObj.get() );
					auto& v = *( stack.end() - 2 );
					if( v.index() == 0 ) // object
						*pValue = std::move( newObj );
					else // array
						std::get< Array* >( v )->emplace_back( std::move( newObj ) );
				}
			}
			if constexpr( std::is_same_v< Rule, Char< '[' > > )
			{
				if( stack.empty() )
				{
					root = ArrayPtr( new Array );
					stack.emplace_back( std::get< ArrayPtr >( root ).get() );
				}
				else
				{
					auto newArray = ArrayPtr( new Array );
					stack.emplace_back( newArray.get() );
					auto& v = *( stack.end() - 2 );
					if( v.index() == 0 ) // object
						*pValue = std::move( newArray );
					else // array
						std::get< Array* >( v )->emplace_back( std::move( newArray ) );
				}
			}
			if constexpr( std::is_same_v< Rule, Char< '}' > > || std::is_same_v< Rule, Char< ']' > > )
				stack.pop_back();
			if constexpr( std::is_same_v< Rule, peg::Name > )
				pValue = &( std::get< Object* >( stack.back() )->insert( std::pair< const std::string, Value >{ sv.substr( 1, sv.size() - 2 ), Value{} } ).first->second );
			if constexpr( std::is_same_v< Rule, peg::IntegerValue > )
			{
				long long v = 0;
				std::from_chars( sv.data(), sv.data() + sv.size(), v );
				addValue( v );
			}
			if constexpr( std::is_same_v< Rule, peg::BooleanValue > )
				addValue( *sv.data() == 't' ? true : false );
			if constexpr( std::is_same_v< Rule, peg::StringValue > )
				addValue( std::string( sv.substr( 1, sv.size() - 2 ) ) );
		}

		template< typename Type >
		inline void addValue( Type value )
		{
			if( stack.back().index() == 0 ) // object
				*pValue = std::move( value );
			else // array
				std::get< Array* >( stack.back() )->emplace_back( std::move( value ) );
		}
	};

} // namespace json_test

using namespace json_test;

TEST_CASE( "JSON test (root is object)", "[json][psm]" )
{
	Parser< peg::Rule, json_test::Action > p;
	static constexpr size_t ParserSize = sizeof( p );
	auto res = p.parse(
	    "{"
	    "\"name\": \"John\","
	    "\"is_alive\": true,"
	    "\"has_car\": false,"
	    "\"age\": 27,"
	    "\"address\": {"
	    "\"street_address\": \"21 2nd Street\","
	    "\"city\": \"New York\"},"
	    "\"phone\":[\"123456789\",\"987654321\"]"
	    "}" );

	Value root = std::move( p.actionFunction().root );
	REQUIRE( root.index() == 3 );

	Object& obj = *std::get< ObjectPtr >( root ).get();
	CHECK( std::get< std::string >( obj.at( "name" ) ) == "John" );
	CHECK( std::get< bool >( obj.at( "is_alive" ) ) == true );
	CHECK( std::get< bool >( obj.at( "has_car" ) ) == false );
	CHECK( std::get< long long >( obj.at( "age" ) ) == 27ll );

	CHECK( obj.at( "address" ).index() == 3 );
	auto addressObj = std::get< ObjectPtr >( obj.at( "address" ) ).get();
	CHECK( std::get< std::string >( addressObj->at( "street_address" ) ) == "21 2nd Street" );
	CHECK( std::get< std::string >( addressObj->at( "city" ) ) == "New York" );

	REQUIRE( obj.at( "phone" ).index() == 4 );
	Array& arr = *std::get< ArrayPtr >( obj.at( "phone" ) ).get();
	CHECK( std::get< std::string >( arr.at( 0 ) ) == "123456789" );
	CHECK( std::get< std::string >( arr.at( 1 ) ) == "987654321" );
}

TEST_CASE( "JSON test (root is array)", "[json][psm]" )
{
	Parser< peg::Rule, json_test::Action > p;
	auto res = p.parse(
	    "[\"hello\",42,{"
	    "\"type\":\"move\","
	    "\"dir\":\"up\""
	    "}]" );

	Value root = std::move( p.actionFunction().root );
	REQUIRE( root.index() == 4 );

	Array& arr = *std::get< ArrayPtr >( root ).get();
	CHECK( std::get< std::string >( arr.at( 0 ) ) == "hello" );
	CHECK( std::get< long long >( arr.at( 1 ) ) == 42ll );
	REQUIRE( arr.at( 2 ).index() == 3 );

	Object& obj = *std::get< ObjectPtr >( arr.at( 2 ) ).get();
	CHECK( std::get< std::string >( obj.at( "type" ) ) == "move" );
	CHECK( std::get< std::string >( obj.at( "dir" ) ) == "up" );
}