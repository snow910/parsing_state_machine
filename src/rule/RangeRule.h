#pragma once

#include "../Rule.h"

namespace psm
{
	template< char Begin, char End >
	class Range : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::NotTrueYet };
			if( *input.current() >= Begin && *input.current() <= End )
			{
				input.consume();
				return { RuleMatchCode::True };
			}
			return { RuleMatchCode::False };
		}
	};

	template< char Begin, char End >
	class NotRange : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.empty() )
				return { RuleMatchCode::NotTrueYet };
			if( *input.current() < Begin || *input.current() > End )
			{
				input.consume();
				return { RuleMatchCode::True };
			}
			return { RuleMatchCode::False };
		}
	};

	class Digit : public Range< '0', '9' >
	{
	};
} // namespace psm