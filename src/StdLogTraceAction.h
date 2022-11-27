#pragma once

#include "Rule.h"
#include <iostream>

namespace psm
{
	struct StdLogTraceAction
	{
		using Rules = std::tuple<>;
		static constexpr bool PsmTracing = true;

		void trace( size_t deep, RuleInputRef input, RuleResult result, std::string_view name )
		{
			std::string_view strCode;
			switch( result.code )
			{
			case RuleMatchCode::True:
				strCode = "True";
				break;
			case RuleMatchCode::CallNested:
				strCode = "CallNested";
				break;
			case RuleMatchCode::False:
				strCode = "False";
				break;
			case RuleMatchCode::TrueCanMore:
				strCode = "TrueCanMore";
				break;
			case RuleMatchCode::NotTrueYet:
				strCode = "NotTrueYet";
				break;
			default:
				break;
			}
			std::clog.width( 3 );
			std::clog << deep << ' ';
			std::clog.width( 12 );
			std::clog << strCode << ' ' << result.callIndex << " \"" << std::string_view( input.begin(), input.position() ) << "\" \"" << name.substr( 0, 128 );
			if( name.size() > 128 )
				std::clog << "...\"\n";
			else
				std::clog << "\"\n";
		}
	};
} // namespace psm