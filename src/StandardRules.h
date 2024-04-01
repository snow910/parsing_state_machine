// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include "rule/AnyRule.h"
#include "rule/CharRule.h"
#include "rule/DiscardRule.h"
#include "rule/EndRule.h"
#include "rule/EndingWithRule.h"
#include "rule/IfRule.h"
#include "rule/NotRule.h"
#include "rule/OneRule.h"
#include "rule/OptRule.h"
#include "rule/PlusRule.h"
#include "rule/RangesRule.h"
#include "rule/RepRule.h"
#include "rule/ReparseRule.h"
#include "rule/SeqRule.h"
#include "rule/StarRule.h"
#include "rule/StrRule.h"
#include "rule/SuccessRule.h"
#include "rule/UntilRule.h"

namespace psm
{
	class PositiveInteger : public Plus< Digit >
	{
	};

	class Integer : public Seq< Opt< One< Char< '+' >, Char< '-' > > >, Plus< Digit > >
	{
	};
} // namespace psm