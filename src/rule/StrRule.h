#pragma once

#include "CharRule.h"

namespace psm
{
	template< char C, char... Cs >
	class Str : public RuleBase
	{
		using Storage = detail::CharsStorage< C, Cs... >;

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			while( !input.empty() )
			{
				if( *input.current() == Storage::array[input.position()] )
					input.consume();
				else
				{
					input.discard();
					return { RuleMatchCode::False };
				}
				if( input.position() == sizeof...( Cs ) + 1 )
					return { RuleMatchCode::True };
			}
			return { RuleMatchCode::NotTrueYet };
		}
	};
} // namespace psm