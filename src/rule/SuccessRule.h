// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

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
