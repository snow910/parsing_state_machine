// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include "Rule.h"
#include <iostream>

namespace psm
{
	struct LogTraceAction
	{
		using Rules = std::tuple< NaR >;
		static constexpr bool PsmTracing = true;

		inline void enableCompactName( bool enabled ) noexcept { compactName_ = enabled; }
		inline void setOutputStream( std::ostream* stream ) noexcept { out_ = stream; }

		void trace( size_t deep, const RuleInput& input, const RuleResult& result, std::string_view name );

	private:
		static std::string_view toStringView( RuleMatchCode code );

	private:
		std::ostream* out_ = &std::clog;
		bool compactName_ = true;
	};
} // namespace psm