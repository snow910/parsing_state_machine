#include "Parser.h"

using namespace psm;

ParserBase::~ParserBase()
{
	for( ; !isRuleStackEmpty(); --currentPos_ )
		popRule( currentPos_->ruleIndex );
}

ParsingResult ParserBase::parse( const char* begin, const char* end, bool complete )
{
	if( begin == end )
	{
		if( !complete )
			return { ParsingResult::Type::Incomplete, { begin, 0 } };
		else if( isRuleStackEmpty() )
			return { ParsingResult::Type::False, { begin, 0 } };
	}

	RuleBase* rule;
	if( isRuleStackEmpty() )
		rule = pushRule( currentPos_->ruleIndex );
	else
		rule = topRule( currentPos_->ruleIndex );
	RuleInput input( begin + ( currentPos_ == pPositions_ ? 0 : ( currentPos_ - 1 )->pos ), end, begin + currentPos_->pos );
	RuleResult result;
	while( true )
	{
		result = rule->match( input, states_ );
		trace( input, result );
		if( result.code == RuleMatchCode::TrueCanMore )
		{
			if( !complete )
			{
				currentPos_->pos = input.current() - begin;
				return { ParsingResult::Type::Incomplete, { begin, 0 } };
			}
			result.code = RuleMatchCode::True;
		}
		else if( result.code == RuleMatchCode::NotTrueYet )
		{
			if( !complete )
			{
				currentPos_->pos = input.current() - begin;
				return { ParsingResult::Type::Incomplete, { begin, 0 } };
			}
			result.code = RuleMatchCode::False;
		}

	ProcessResult:
		if( result.code == RuleMatchCode::CallNested )
		{
			if( currentPos_ + 1 == pPositionsEnd_ )
			{
				reset();
				return { ParsingResult::Type::False, { begin, 0 } };
			}
			currentPos_->nestedIndex = result.callIndex;
			currentPos_->pos = input.current() - begin;
			std::size_t ruleIndex = currentPos_->ruleIndex;
			++currentPos_;
			currentPos_->ruleIndex = indexForNestedRule( ruleIndex, result.callIndex );
			currentPos_->pos = input.current() - begin;
			rule = pushRule( currentPos_->ruleIndex );
			input = RuleInput( input.current(), input.end() );
			if( rule == nullptr )
			{
				--currentPos_;
				reset();
				return { ParsingResult::Type::False, { begin, 0 } };
			}
			continue;
		}
		else
		{
			if( currentPos_ == pPositions_ )
			{
				if( result.code == RuleMatchCode::True )
				{
					callAction( rule, 0, std::string_view( begin, input.position() ) );
					popRule( currentPos_->ruleIndex );
					currentPos_->pos = 0;
					return { input.position() > 0 ? ParsingResult::Type::True : ParsingResult::Type::False, std::string_view( begin, input.position() ) };
				}
				popRule( currentPos_->ruleIndex );
				currentPos_->pos = 0;
				return { ParsingResult::Type::False, { begin, 0 } };
			}
			if( result.code == RuleMatchCode::True )
			{
				std::size_t beginPos = ( currentPos_ - 1 )->pos;
				callAction( rule, beginPos, std::string_view( begin + beginPos, input.position() ) );
			}
			RuleInput nestedInput = input;
			NestedRuleResult nestedResult( nestedInput );
			nestedResult.rule = rule;
			auto prevPos = currentPos_ - 1;
			auto prevRule = previousRule( currentPos_->ruleIndex, prevPos->ruleIndex );
			nestedResult.index = prevPos->nestedIndex;
			nestedResult.result = result.code == RuleMatchCode::True;
			input = RuleInput( begin + ( prevPos == pPositions_ ? 0 : ( prevPos - 1 )->pos ), end, begin + prevPos->pos );
			result = prevRule->nestedResult( input, states_, nestedResult );
			popRule( currentPos_->ruleIndex );
			rule = prevRule;
			currentPos_ = prevPos;
			trace( input, result );
			if( result.code == RuleMatchCode::TrueCanMore )
			{
				if( complete )
				{
					result.code = RuleMatchCode::True;
					goto ProcessResult;
				}
				currentPos_->pos = input.position();
				return { ParsingResult::Type::Incomplete, { begin, 0 } };
			}
			else if( result.code == RuleMatchCode::NotTrueYet )
			{
				if( complete )
				{
					result.code = RuleMatchCode::False;
					goto ProcessResult;
				}
				currentPos_->pos = input.position();
				return { ParsingResult::Type::Incomplete, { begin, 0 } };
			}
			goto ProcessResult;
		}
	}
	return { ParsingResult::Type::False, { begin, 0 } };
}

void ParserBase::reset()
{
	for( ; !isRuleStackEmpty(); --currentPos_ )
		popRule( currentPos_->ruleIndex );
	currentPos_->pos = 0;
}

RuleBase* ParserBase::pushRule( std::size_t index )
{
	if( stackTop_ + pRuleInfos_[index].constructor->size() > ruleStackEnd_ )
		return nullptr;
	RuleBase* rule = pRuleInfos_[index].constructor->construct( stackTop_ );
	stackTop_ += pRuleInfos_[index].constructor->size();
	if( pRuleInfos_[index].constructor->isQuiet() )
		++quietCounter_;
	return rule;
}

void ParserBase::popRule( std::size_t index )
{
	stackTop_ -= pRuleInfos_[index].constructor->size();
	reinterpret_cast< RuleBase* >( stackTop_ )->~RuleBase();
	if( pRuleInfos_[index].constructor->isQuiet() )
		--quietCounter_;
}

RuleBase* ParserBase::topRule( std::size_t index )
{
	return reinterpret_cast< RuleBase* >( stackTop_ - pRuleInfos_[index].constructor->size() );
}

RuleBase* ParserBase::previousRule( std::size_t topIntex, std::size_t prevIndex )
{
	return reinterpret_cast< RuleBase* >( stackTop_ - pRuleInfos_[topIntex].constructor->size() - pRuleInfos_[prevIndex].constructor->size() );
}

bool ParserBase::isRuleStackEmpty() const noexcept
{
	return stackTop_ == ruleStackBegin_;
}
