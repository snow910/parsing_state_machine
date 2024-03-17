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
			return { ParsingStatus::Incomplete, { begin, 0 } };
		else if( isRuleStackEmpty() )
			return { ParsingStatus::Fail, { begin, 0 } };
	}

	RuleBase* rule;
	if( isRuleStackEmpty() )
		rule = pushRule( currentPos_->ruleIndex );
	else
		rule = topRule( currentPos_->ruleIndex );
	RuleInput input( begin + ( currentPos_ == pPositions_ ? 0 : ( currentPos_ - 1 )->pos ), end, begin + currentPos_->pos );
	RuleResult result;
	std::size_t extSize = end - begin;
	bool extComplete = complete;
	Position* matchPos = nullptr;
	while( true )
	{
		result = rule->match( input, states_ );
		trace( input, result );
		if( result.code == RuleMatchCode::TrueCanMore )
		{
			if( !complete )
			{
				currentPos_->pos = input.current() - begin;
				return { ParsingStatus::Incomplete, { begin, 0 } };
			}
			result.code = RuleMatchCode::True;
		}
		else if( result.code == RuleMatchCode::NotTrueYet )
		{
			if( !complete )
			{
				currentPos_->pos = input.current() - begin;
				return { ParsingStatus::Incomplete, { begin, 0 } };
			}
			result.code = RuleMatchCode::False;
		}

	ProcessResult:
		if( result.code == RuleMatchCode::CallNested || result.code == RuleMatchCode::CallNestedOnMatch )
		{
			if( currentPos_ + 1 == pPositionsEnd_ )
			{
				reset();
				return { ParsingStatus::Overflow, { begin, 0 } };
			}
			currentPos_->nestedIndex = ( uint16_t )result.callIndex;
			currentPos_->pos = input.current() - begin;
			if( result.code == RuleMatchCode::CallNested )
			{
				currentPos_->callOnMatch = false;
				input = RuleInput( input.current(), input.end() );
			}
			else
			{
				currentPos_->callOnMatch = true;
				input = RuleInput( input.begin(), input.current() );
				if( !matchPos )
				{
					matchPos = currentPos_;
					complete = true;
				}
			}
			std::size_t ruleIndex = currentPos_->ruleIndex;
			( ++currentPos_ )->ruleIndex = ( uint16_t )indexForNestedRule( ruleIndex, result.callIndex );
			rule = pushRule( currentPos_->ruleIndex );
			if( rule == nullptr )
			{
				--currentPos_;
				reset();
				return { ParsingStatus::Overflow, { begin, 0 } };
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
					return { input.position() > 0 ? ParsingStatus::Success : ParsingStatus::Fail, std::string_view( begin, input.position() ) };
				}
				popRule( currentPos_->ruleIndex );
				currentPos_->pos = 0;
				return { ParsingStatus::Fail, { begin, 0 } };
			}
			if( result.code == RuleMatchCode::True )
			{
				if( !callAction( rule, input.begin() - begin, std::string_view( input.begin(), input.position() ) ) )
				{
					reset();
					return { ParsingStatus::Aborted, std::string_view( input.begin(), input.position() ) };
				}
			}
			RuleInput nestedInput = input;
			NestedRuleResult nestedResult( nestedInput );
			nestedResult.rule = rule;
			auto prevPos = currentPos_ - 1;
			auto prevRule = previousRule( currentPos_->ruleIndex, prevPos->ruleIndex );
			nestedResult.index = prevPos->nestedIndex;
			nestedResult.result = result.code == RuleMatchCode::True;
			input = RuleInput( begin + inputBegin( prevPos ), begin + inputEnd( prevPos, matchPos, extSize ), begin + prevPos->pos );
			result = prevRule->nestedResult( input, states_, nestedResult );
			popRule( currentPos_->ruleIndex );
			rule = prevRule;
			currentPos_ = prevPos;
			trace( input, result );
			if( matchPos == currentPos_ )
			{
				complete = extComplete;
				matchPos = nullptr;
			}
			if( result.code == RuleMatchCode::TrueCanMore )
			{
				if( complete )
				{
					result.code = RuleMatchCode::True;
					goto ProcessResult;
				}
				currentPos_->pos = input.current() - begin;
				return { ParsingStatus::Incomplete, { begin, 0 } };
			}
			else if( result.code == RuleMatchCode::NotTrueYet )
			{
				if( complete )
				{
					result.code = RuleMatchCode::False;
					goto ProcessResult;
				}
				currentPos_->pos = input.current() - begin;
				return { ParsingStatus::Incomplete, { begin, 0 } };
			}
			goto ProcessResult;
		}
	}
	return { ParsingStatus::Fail, { begin, 0 } };
}

void ParserBase::reset()
{
	for( ; !isRuleStackEmpty(); --currentPos_ )
		popRule( currentPos_->ruleIndex );
	( ++currentPos_ )->pos = 0;
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

std::size_t ParserBase::inputBegin( Position* pos ) noexcept
{
	if( pos == pPositions_ )
		return 0;
	for( --pos;; --pos )
	{
		if( !pos->callOnMatch )
			return pos->pos;
		if( pos == pPositions_ )
			return 0;
	}
}

std::size_t ParserBase::inputEnd( Position* pos, Position* matchPos, std::size_t extSize ) noexcept
{
	if( !matchPos || pos == matchPos )
		return extSize;
	for( --pos;; --pos )
	{
		if( pos->callOnMatch )
			return pos->pos;
		if( pos == matchPos )
			return extSize;
	}
}