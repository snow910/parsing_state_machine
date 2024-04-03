// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

#include "LogTraceAction.h"
#include "Parser.h"
#include "StandardRules.h"

#if !defined( _MSC_VER )
	#include <cxxabi.h>
	#include <memory>
#endif

using namespace psm;

void LogTraceAction::trace( size_t deep, const RuleInput& input, const RuleResult& result, std::string_view name )
{
#if !defined( _MSC_VER )
	int status = -4;
	std::unique_ptr< char, void ( * )( void* ) > str( abi::__cxa_demangle( name.data(), nullptr, nullptr, &status ), std::free );
	if( status == 0 )
		name = std::string_view( str.get() );
#endif
	out_->width( 3 );
	*out_ << deep << ' ';
	out_->width( 12 );
	*out_ << toStringView( result.code ) << ' ' << result.callIndex << " \"" << std::string_view( input.begin(), input.position() ) << "\" \"";
	if( !compactName_ )
	{
		if( name.size() > 128 )
			*out_ << name.substr( 0, 128 ) << "...\"\n";
		else
			*out_ << name << "\"\n";
		return;
	}

	using Class = Str< 'c', 'l', 'a', 's', 's', ' ' >;
	using Struct = Str< 's', 't', 'r', 'u', 'c', 't', ' ' >;
	using Identifier = Seq< One< Char< '_' >, Ranges< 'a', 'z', 'A', 'Z' > >, Star< One< Char< '_' >, Ranges< 'a', 'z', 'A', 'Z', '0', '9' > > > >;
	using Namespace = Seq< Opt< Class, Struct >, Plus< Seq< Identifier, Str< ':', ':' > > > >;
	using Spaces = Plus< Char< ' ' > >;
	using Symbol = Str< '(', 'c', 'h', 'a', 'r', ')' >;
	using Ignore = One< Namespace, Spaces, Symbol, End >;
	using Rule = Star< Seq< EndingWith< Ignore, SuccessAll >, Not< End > > >;
	size_t n = 128;
	auto res = parseStringView< Rule, std::tuple< SuccessAll > >(
	    name,
	    [&n, this]< typename Type >( const Type&, std::size_t, std::string_view match )
	    {
		    if( n < match.size() )
		    {
			    *out_ << match.substr( 0, n ) << "...\"\n";
			    return false;
		    }
		    *out_ << match;
		    n -= match.size();
		    return true;
	    } );
	if( res.status == ParsingStatus::Success )
		*out_ << "\"\n";
}

std::string_view LogTraceAction::toStringView( RuleMatchCode code )
{
	switch( code )
	{
	case RuleMatchCode::True:
		return "True";
	case RuleMatchCode::CallNested:
		return "CallNested";
	case RuleMatchCode::CallNestedOnMatch:
		return "CallNested+";
	case RuleMatchCode::False:
		return "False";
	case RuleMatchCode::TrueCanMore:
		return "TrueCanMore";
	case RuleMatchCode::NotTrueYet:
		return "NotTrueYet";
	default:
		return "Unknown";
	}
}