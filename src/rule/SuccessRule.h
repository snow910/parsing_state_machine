#pragma once

#include "../Rule.h"

namespace psm
{
	class Success : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::True };
		}
	};

	class SuccessAll : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			input.setCurrent( input.end() );
			return { RuleMatchCode::True };
		}
	};
} // namespace psm
