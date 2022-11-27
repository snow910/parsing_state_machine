#pragma once

#include "AnyRule.h"

namespace psm
{
	template< std::size_t Min, std::size_t Max, typename Rule >
	class RepMinMax : public RuleBase
	{
		static_assert( Min <= Max && Max != 0, "wrong parameters" );
		std::size_t n_ = 0;

	public:
		using Rules = std::tuple< Rule >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
			{
				input.setCurrent( nestedResult.input.current() );
				if( ++n_ == Max )
					return { RuleMatchCode::True };
				return { RuleMatchCode::CallNested, 0 };
			}
			if( n_ >= Min )
				return { RuleMatchCode::True };
			return { RuleMatchCode::False };
		}
	};

	template< std::size_t Min, std::size_t Max >
	class RepMinMax< Min, Max, Any > : public RuleBase
	{
		static_assert( Min <= Max && Max != 0, "wrong parameters" );
		std::size_t n_ = 0;

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.consume( Max - input.position() ) )
				return { RuleMatchCode::True };
			if( input.position() >= Min )
				return { RuleMatchCode::True };
			return { RuleMatchCode::NotTrueYet };
		}
	};

	template< std::size_t Min, typename Rule >
	class RepMin : public RepMinMax< Min, ( std::size_t )-1, Rule >
	{
	};

	template< std::size_t Max, typename Rule >
	class RepMax : public RepMinMax< 0, Max, Rule >
	{
	};

	template< typename Rule >
	class Rep : public RepMinMax< 0, ( std::size_t )-1, Rule >
	{
	};
} // namespace psm