#pragma once

#include "Parser.h"
#include "StandardRules.h"
#include <iostream>

namespace psm
{
	struct LogTraceAction
	{
		using Rules = std::tuple< NaR >;
		static constexpr bool PsmTracing = true;

		inline void enableCompactName( bool enabled ) noexcept { compactName_ = enabled; }
		inline void setOutputStream( std::ostream* stream ) noexcept { out_ = stream; }

		void trace( size_t deep, const RuleInput& input, const RuleResult& result, std::string_view name )
		{
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

			using Class = Str< 'c', 'l', 'a', 's', 's' >;
			using Struct = Str< 's', 't', 'r', 'u', 'c', 't' >;
			using Identifier = Seq< One< Char< '_' >, Range< 'a', 'z' >, Range< 'A', 'Z' > >, Star< One< Char< '_' >, Range< 'a', 'z' >, Range< 'A', 'Z' >, Range< '0', '9' > > > >;
			Parser< One< Seq< One< Class, Struct >, Char< ' ' >, Plus< Seq< Identifier, Str< ':', ':' > > > >, Plus< Char< ' ' > > > > parser;
			const char* begin = name.data();
			const char* end = &name.back() + 1;
			const char* p = begin;
			size_t n = 0;
			for( ; p != end && ( p - begin ) + n < 128; ++p )
			{
				auto res = parser.parse( p, end );
				if( res.status == ParsingStatus::Fail )
					continue;
				*out_ << std::string_view( begin, p );
				n += p - begin;
				p = begin = res.match.data() + res.match.size();
			}
			if( ( p - begin ) + n >= 128 )
				*out_ << std::string_view( begin, p ).substr( 0, 128 - n ) << "...\"\n";
			else
				*out_ << std::string_view( begin, p ) << "\"\n";
		}

	private:
		static std::string_view toStringView( RuleMatchCode code )
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

	private:
		std::ostream* out_ = &std::clog;
		bool compactName_ = true;
	};
} // namespace psm