#pragma once

#include "../Rule.h"
#include <cstring>

namespace psm
{
	template< char C >
	class Char : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::NotTrueYet };
			if( *input.current() == C )
			{
				input.consume();
				return { RuleMatchCode::True };
			}
			return { RuleMatchCode::False };
		}
	};

	template< char C >
	class NotChar : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::NotTrueYet };
			if( *input.current() != C )
			{
				input.consume();
				return { RuleMatchCode::True };
			}
			return { RuleMatchCode::False };
		}
	};

	template< char C, char... Cs >
	class Str : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			static constexpr std::array< const char, sizeof...( Cs ) + 1 > str{ C, Cs... };
			if( str.size() - input.position() > input.size() )
				return { RuleMatchCode::NotTrueYet };
			if( strncmp( str.data(), input.current(), str.size() ) == 0 )
			{
				input.consume( str.size() );
				return { RuleMatchCode::True };
			}
			return { RuleMatchCode::False };
		}
	};
} // namespace psm