#pragma once

#include "../Rule.h"

namespace psm
{
	template< typename Condition, typename Then, typename Else = NaR >
	class If : public RuleBase
	{
	public:
		static_assert( !( std::is_same_v< Then, NaR > && std::is_same_v< Else, NaR > ), "Then and Else are both NaR" );
		using Rules = std::conditional_t< std::is_same_v< Else, NaR >, std::tuple< Condition, Then >,
						  std::conditional_t< std::is_same_v< Then, NaR >, std::tuple< Condition, Else >, std::tuple< Condition, Then, Else > > >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.index == 0 )
			{
				if( nestedResult.result )
				{
					input.setCurrent( nestedResult.input.current() );
					if constexpr( std::is_same_v< Then, NaR > )
						return { RuleMatchCode::True };
					return { RuleMatchCode::CallNested, 1 };
				}
				else
				{
					if constexpr( std::is_same_v< Else, NaR > )
						return { RuleMatchCode::True };
					return { RuleMatchCode::CallNested, 2 };
				}
			}
			if( nestedResult.result )
			{
				input.setCurrent( nestedResult.input.current() );
				return { RuleMatchCode::True };
			}
			else
				return { RuleMatchCode::False };
		}
	};
} // namespace psm