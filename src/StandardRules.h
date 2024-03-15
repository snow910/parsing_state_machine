#pragma once

#include "rule/AnyRule.h"
#include "rule/CharRule.h"
#include "rule/DiscardRule.h"
#include "rule/EndRule.h"
#include "rule/IfRule.h"
#include "rule/NotRule.h"
#include "rule/OneRule.h"
#include "rule/OptRule.h"
#include "rule/PlusRule.h"
#include "rule/RangeRule.h"
#include "rule/RepRule.h"
#include "rule/ReparseRule.h"
#include "rule/SeqRule.h"
#include "rule/StarRule.h"
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