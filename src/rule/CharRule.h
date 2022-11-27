#pragma once

#include "../Rule.h"

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
			static std::array< const char, sizeof...( Cs ) + 1 > str{ C, Cs... };
			if( str.size() - input.position() > input.size() )
				return { RuleMatchCode::NotTrueYet };
			for( auto c : str )
			{
				if( c != *input.current() )
					return { RuleMatchCode::False };
				input.consume();
			}
			return { RuleMatchCode::True };
		}
	};
} // namespace psm