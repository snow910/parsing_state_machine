#pragma once

#include "../Rule.h"

namespace psm
{
	class Any : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.consume() )
				return { RuleMatchCode::True };
			return { RuleMatchCode::NotTrueYet };
		}
	};

	template< std::size_t N >
	class AnyN : public RuleBase
	{
		static_assert( N != 0, "wrong parameter" );

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.consume( N - input.position() ) )
				return { RuleMatchCode::True };
			return { RuleMatchCode::NotTrueYet };
		}
	};
} // namespace psm