#pragma once

#include <array>
#include <assert.h>
#include <span>
#include <stdint.h>
#include <tuple>
#include <type_traits>

// Parsing state machine
namespace psm
{
	class RuleInput
	{
		const char *begin_, *end_, *current_;

	public:
		inline RuleInput( const char* begin, const char* end ) noexcept
		    : begin_( begin ), end_( end ), current_( begin ) {}
		inline RuleInput( const char* begin, const char* end, const char* current ) noexcept
		    : begin_( begin ), end_( end ), current_( current ) {}

		inline const char* begin() const noexcept { return begin_; }
		inline const char* current() const noexcept { return current_; }
		inline const char* end() const noexcept { return end_; }
		inline std::size_t size() const noexcept { return ( std::size_t )( end_ - current_ ); }
		inline bool empty() const noexcept { return current_ == end_; }
		inline std::size_t position() const noexcept { return ( std::size_t )( current_ - begin_ ); }
		inline void setCurrent( const char* current ) noexcept
		{
			current_ = current;
			if( current_ > end_ )
				current_ = end_;
		}
		inline bool consume( std::size_t size ) noexcept
		{
			current_ += size;
			if( current_ > end_ )
			{
				current_ = end_;
				return false;
			}
			return true;
		}
		inline bool consume() noexcept
		{
			if( current_ != end_ )
			{
				++current_;
				return true;
			}
			return false;
		}
		inline void discard() noexcept { current_ = begin_; }
	};

	class RuleInputRef
	{
		RuleInput& ref_;

	public:
		inline RuleInputRef( RuleInput& ref ) : ref_( ref ) {}

		inline const char* begin() const noexcept { return ref_.begin(); }
		inline const char* current() const noexcept { return ref_.current(); }
		inline const char* end() const noexcept { return ref_.end(); }
		inline std::size_t size() const noexcept { return ref_.size(); }
		inline bool empty() const noexcept { return ref_.empty(); }
		inline std::size_t position() const noexcept { return ref_.position(); }
		inline void setCurrent( const char* current ) noexcept { ref_.setCurrent( current ); }
		inline bool consume( std::size_t size ) noexcept { return ref_.consume( size ); }
		inline bool consume() noexcept { return ref_.consume(); }
		inline void discard() noexcept { ref_.discard(); }
	};

	enum class RuleMatchCode
	{
		True,
		False,
		TrueCanMore,
		NotTrueYet,
		CallNested
	};

	struct RuleResult
	{
		RuleMatchCode code;
		std::size_t callIndex = 0;
	};

	class RuleBase;

	struct NestedRuleResult
	{
		NestedRuleResult( RuleInput& input_ ) : input( input_ ) {}
		RuleBase* rule;
		std::size_t index;
		RuleInput& input;
		bool result;
	};

	class RuleBase
	{
	public:
		using Rules = std::tuple<>;

		virtual ~RuleBase() {}
		virtual RuleResult match( RuleInputRef input, void* states )
		{
			return { RuleMatchCode::False };
		}
		virtual RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult )
		{
			return { RuleMatchCode::False };
		}
	};

	namespace detail
	{
		template< typename Type, Type V, Type... Vs >
		struct _sum
		{
			static constexpr Type value = V + _sum< Type, Vs... >::value;
		};

		template< typename Type, Type V >
		struct _sum< Type, V >
		{
			static constexpr Type value = V;
		};

		template< typename Type, std::size_t... Vs >
		struct sum
		{
			static constexpr Type value = _sum< Type, Vs... >::value;
		};

		template< typename Type, Type V, Type... Vs >
		struct _max
		{
			static constexpr Type value = V > _max< Type, Vs... >::value ? V : _max< Type, Vs... >::value;
		};

		template< typename Type, Type V >
		struct _max< Type, V >
		{
			static constexpr Type value = V;
		};

		template< typename Type, std::size_t... Vs >
		struct max
		{
			static constexpr Type value = _max< Type, Vs... >::value;
		};

		template< typename S >
		struct integer_sequence_sum;

		template< typename T, T... Vs >
		struct integer_sequence_sum< std::integer_sequence< T, Vs... > > : sum< T, Vs... >
		{
		};

		template< std::size_t Id, typename T, typename T0, typename... Ts >
		struct _tuple_element_index
		{
			static constexpr std::size_t value = std::is_same_v< T, T0 > ? Id : _tuple_element_index< Id + 1, T, Ts... >::value;
		};

		template< std::size_t Id, typename T, typename T0 >
		struct _tuple_element_index< Id, T, T0 >
		{
			static constexpr std::size_t value = std::is_same_v< T, T0 > ? Id : ( std::size_t )-1;
		};

		template< typename T, typename Tp >
		struct tuple_element_index;

		template< typename T, typename... Ts >
		struct tuple_element_index< T, std::tuple< Ts... > > : _tuple_element_index< 0, T, Ts... >
		{
		};

		template< typename T >
		struct tuple_element_index< T, std::tuple<> > : std::integral_constant< std::size_t, ( std::size_t )-1 >
		{
		};

		template< typename Whole, typename Part >
		struct index_sequence_for_part_in_whole;

		template< typename Whole, typename... Part >
		struct index_sequence_for_part_in_whole< Whole, std::tuple< Part... > >
		{
			using type = std::index_sequence< tuple_element_index< Part, Whole >::value... >;
		};

		template< typename Whole, typename Part >
		using index_sequence_for_part_in_whole_t = typename index_sequence_for_part_in_whole< Whole, Part >::type;

		template< typename Tuple, typename NewType >
		struct push_type_back;

		template< typename... Ts, typename NewType >
		struct push_type_back< std::tuple< Ts... >, NewType >
		{
			using type = std::tuple< Ts..., NewType >;
		};

		template< typename Tuple, typename NewType >
		using push_type_back_t = typename push_type_back< Tuple, NewType >::type;

		template< typename Tuple, typename NewType >
		struct push_type_front;

		template< typename... Ts, typename NewType >
		struct push_type_front< std::tuple< Ts... >, NewType >
		{
			using type = std::tuple< NewType, Ts... >;
		};

		template< typename Tuple, typename NewType >
		using push_type_front_t = typename push_type_front< Tuple, NewType >::type;

		template< typename Unique, typename Others >
		struct push_unique_types_back;

		template< typename Unique, typename Other, typename... Others >
		struct push_unique_types_back< Unique, std::tuple< Other, Others... > >
		    : std::conditional_t< tuple_element_index< Other, Unique >::value == ( std::size_t )-1,
					  push_unique_types_back< push_type_back_t< Unique, Other >, std::tuple< Others... > >,
					  push_unique_types_back< Unique, std::tuple< Others... > > >
		{
		};

		template< typename Unique, typename Other >
		struct push_unique_types_back< Unique, std::tuple< Other > >
		{
			using type = std::conditional_t< tuple_element_index< Other, Unique >::value == ( std::size_t )-1,
							 push_type_back_t< Unique, Other >,
							 Unique >;
		};

		template< typename Unique >
		struct push_unique_types_back< Unique, std::tuple<> >
		{
			using type = Unique;
		};

		template< typename Unique, typename Others >
		using push_unique_types_back_t = typename push_unique_types_back< Unique, Others >::type;

		template< typename Tuple >
		using unique_types = push_unique_types_back< std::tuple<>, Tuple >;

		template< typename Tuple >
		using unique_types_t = typename unique_types< Tuple >::type;

		template< typename Result, typename... Tuples >
		struct _unique_tuple_cat;

		template< typename Result, typename Tuple, typename... Others >
		struct _unique_tuple_cat< Result, Tuple, Others... > : _unique_tuple_cat< push_unique_types_back_t< Result, Tuple >, Others... >
		{
		};

		template< typename Result, typename Tuple >
		struct _unique_tuple_cat< Result, Tuple > : push_unique_types_back< Result, Tuple >
		{
		};

		template< typename... Tuples >
		struct unique_tuple_cat;

		template< typename Tuple, typename... Others >
		struct unique_tuple_cat< Tuple, Others... > : _unique_tuple_cat< Tuple, Others... >
		{
		};

		template< typename Tuple >
		struct unique_tuple_cat< Tuple >
		{
			using type = Tuple;
		};

		template<>
		struct unique_tuple_cat<>
		{
			using type = std::tuple<>;
		};

		template< typename... Tuples >
		using unique_tuple_cat_t = typename unique_tuple_cat< Tuples... >::type;

		//===========================================================================================================================================

		template< std::size_t Deep, std::size_t StackSize, typename Rules >
		struct _rules_info;

		template< std::size_t Deep, std::size_t StackSize, typename... Rules_ >
		struct _rules_info< Deep, StackSize, std::tuple< Rules_... > >
		{
			static constexpr std::size_t max_deep = max< std::size_t, _rules_info< Deep + 1, StackSize + sizeof( Rules_ ), typename Rules_::Rules >::max_deep... >::value;
			static constexpr std::size_t stack_size = max< std::size_t, _rules_info< Deep + 1, StackSize + sizeof( Rules_ ), typename Rules_::Rules >::stack_size... >::value;
			using unique_rules = unique_tuple_cat_t< typename _rules_info< Deep + 1, StackSize + sizeof( Rules_ ), typename Rules_::Rules >::unique_rules..., unique_types_t< std::tuple< Rules_... > > >;
		};

		template< std::size_t Deep, std::size_t StackSize >
		struct _rules_info< Deep, StackSize, std::tuple<> >
		{
			static constexpr std::size_t max_deep = Deep;
			static constexpr std::size_t stack_size = StackSize;
			using unique_rules = std::tuple<>;
		};

		template< typename Rule >
		struct rule_info : _rules_info< ( std::size_t )-1, 0, std::tuple< Rule > >
		{
		};

		template< typename Rules >
		struct dependent_rule_count;

		template< typename... Rules_ >
		struct dependent_rule_count< std::tuple< Rules_... > >
		    : std::integral_constant< std::size_t, integer_sequence_sum< std::index_sequence< std::tuple_size_v< typename Rules_::Rules >... > >::value >
		{
		};

		struct RuleConstructorBase
		{
			virtual RuleBase* construct( void* mem ) const noexcept = 0;
			virtual std::size_t size() const noexcept = 0;
		};

		template< typename Rule >
		struct RuleConstructor : RuleConstructorBase
		{
			RuleBase* construct( void* mem ) const noexcept override
			{
				return new( mem ) Rule;
			}
			std::size_t size() const noexcept override
			{
				return sizeof( Rule );
			}
		};

		template< typename Rule >
		struct RuleConstructorStorage
		{
			static RuleConstructor< Rule > constructor;
		};

		template< typename Rule >
		RuleConstructor< Rule > RuleConstructorStorage< Rule >::constructor;

		template< typename Rule, typename Action >
		void ruleActionFunction( Action& action, const RuleBase* rule, std::size_t beginPos, const char* begin, const char* end )
		{
			action( static_cast< const Rule& >( *rule ), beginPos, begin, end );
		}

		template< typename Rule, typename Action >
		void ruleActionFunctionPos( Action& action, const std::span< const std::size_t >& rulePos, const RuleBase* rule, std::size_t beginPos, const char* begin, const char* end )
		{
			action( rulePos, static_cast< const Rule& >( *rule ), beginPos, begin, end );
		}

		struct Nothing
		{
			using Rules = std::tuple<>;
		};

		struct NaR // not a rule
		{
		};
	} // namespace detail

	struct ParsingResult
	{
		enum class Type
		{
			True,
			False,
			Incomplete
		} type;
		const char* after;
	};

	class ParserBase
	{
	public:
		virtual ~ParserBase() {}

		virtual ParsingResult parse( const char* begin, const char* end, bool complete = true ) = 0;
		virtual void reset() = 0;

		template< std::size_t N >
		inline ParsingResult parseString( const char ( &str )[N] ) { return parse( str, str + N - 1 ); }
	};

	// ActionFunction is a functional object type that contains Rules (a std::tuple alias) of interesting rules
	template< typename Rule, typename ActionFunction = detail::Nothing, bool RulePositioning = false >
	class Parser : public ParserBase
	{
	public:
		using UniqueRules = typename detail::rule_info< Rule >::unique_rules;
		static constexpr std::size_t MaxDeep = detail::rule_info< Rule >::max_deep;
		static constexpr bool UseActionFunction = std::tuple_size_v< typename ActionFunction::Rules > != 0;
		static constexpr bool UseRulePosition = UseActionFunction && RulePositioning;

		Parser();
		Parser( ActionFunction func );
		Parser( const Parser& ) = delete;
		Parser( Parser&& ) = delete;
		~Parser();

		inline void setActionFunction( ActionFunction func );
		inline void setStates( void* states );

		ParsingResult parse( const char* begin, const char* end, bool complete = true ) override;
		void reset() override;

	private:
		static constexpr std::size_t IndexArraySize = detail::dependent_rule_count< UniqueRules >::value;
		using IndexType = std::conditional_t< IndexArraySize <= 256, uint8_t, uint16_t >;
		struct Position;

		template< std::size_t Id >
		inline void initIndex( std::size_t& n );

		template< std::size_t... Ids >
		inline void initIndexes( std::size_t& n, std::index_sequence< Ids... > );

		template< std::size_t Id >
		inline void initRule( std::size_t& n );

		template< std::size_t... Ids >
		void initRuleInfos( std::index_sequence< Ids... > );

		void callAction( Position* pos, RuleBase* rule, std::size_t beginPos, const char* begin, const char* end );
		RuleBase* pushRule( std::size_t index );
		void popRule( std::size_t index );
		RuleBase* topRule( std::size_t index );
		RuleBase* previousRule( std::size_t topIntex, std::size_t prevIndex );
		bool isRuleStackEmpty() const noexcept;

	private:
		using _RuleActionFunction = void ( * )( ActionFunction&, const RuleBase*, std::size_t, const char*, const char* );
		using _RuleActionFunctionPos = void ( * )( ActionFunction&, const std::span< const std::size_t >&, const RuleBase*, std::size_t, const char*, const char* );
		using RuleActionFunction = std::conditional_t< UseRulePosition, _RuleActionFunctionPos, _RuleActionFunction >;
		struct Position
		{
			std::size_t ruleIndex;
			std::size_t nestedIndex = 0;
			std::size_t pos;
		};
		struct _RuleInfoV0
		{
			detail::RuleConstructorBase* constructor;
			IndexType* nestedRuleIndexes;
		};
		struct _RuleInfoV1
		{
			detail::RuleConstructorBase* constructor;
			IndexType* nestedRuleIndexes;
			RuleActionFunction func;
		};
		using RuleInfo = std::conditional_t< UseActionFunction, _RuleInfoV1, _RuleInfoV0 >;
		ActionFunction actionFunc_;
		void* states_ = nullptr;
		std::array< RuleInfo, std::tuple_size_v< UniqueRules > > ruleInfos_;
		std::array< IndexType, IndexArraySize > indexes_;
		std::array< Position, detail::rule_info< Rule >::max_deep + 1 > positions_;
		std::array< uint8_t, detail::rule_info< Rule >::stack_size > ruleStackData_;
		Position* currentPos_;
		uint8_t* stackTop_;
	};

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	Parser< Rule, ActionFunction, RulePositioning >::Parser()
	{
		initRuleInfos( std::make_index_sequence< std::tuple_size_v< UniqueRules > >{} );
		currentPos_ = &positions_.front();
		currentPos_->pos = 0;
		currentPos_->ruleIndex = std::tuple_size_v< UniqueRules > - 1;
		stackTop_ = &ruleStackData_.front();
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	Parser< Rule, ActionFunction, RulePositioning >::Parser( ActionFunction func ) : actionFunc_( std::move( func ) )
	{
		initRuleInfos( std::make_index_sequence< std::tuple_size_v< UniqueRules > >{} );
		currentPos_ = &positions_.front();
		currentPos_->pos = 0;
		currentPos_->ruleIndex = std::tuple_size_v< UniqueRules > - 1;
		stackTop_ = &ruleStackData_.front();
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	Parser< Rule, ActionFunction, RulePositioning >::~Parser()
	{
		for( ; !isRuleStackEmpty(); --currentPos_ )
			popRule( currentPos_->ruleIndex );
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	inline void Parser< Rule, ActionFunction, RulePositioning >::setActionFunction( ActionFunction func )
	{
		actionFunc_ = std::move( func );
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	inline void Parser< Rule, ActionFunction, RulePositioning >::setStates( void* states )
	{
		states_ = states;
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	ParsingResult Parser< Rule, ActionFunction, RulePositioning >::parse( const char* begin, const char* end, bool complete )
	{
		if( begin == end )
		{
			if( !complete )
				return { ParsingResult::Type::Incomplete, begin };
			else if( isRuleStackEmpty() )
				return { ParsingResult::Type::False, begin };
		}

		RuleBase* rule;
		if( isRuleStackEmpty() )
			rule = pushRule( currentPos_->ruleIndex );
		else
			rule = topRule( currentPos_->ruleIndex );
		RuleInput input( begin + ( currentPos_ == &positions_.front() ? 0 : ( currentPos_ - 1 )->pos ), end, begin + currentPos_->pos );
		RuleResult result;
		while( true )
		{
			if( input.size() )
			{
				result = rule->match( input, states_ );
				if( result.code == RuleMatchCode::TrueCanMore )
				{
					if( !complete )
					{
						currentPos_->pos = input.current() - begin;
						return { ParsingResult::Type::Incomplete, begin };
					}
					result.code = RuleMatchCode::True;
				}
				else if( result.code == RuleMatchCode::NotTrueYet )
				{
					if( !complete )
					{
						currentPos_->pos = input.current() - begin;
						return { ParsingResult::Type::Incomplete, begin };
					}
					result.code = RuleMatchCode::False;
				}
			}
			else if( complete )
				result.code = RuleMatchCode::False;
			else
			{
				currentPos_->pos = input.current() - begin;
				return { ParsingResult::Type::Incomplete, begin };
			}

		ProcessResult:
			if( result.code == RuleMatchCode::CallNested )
			{
				currentPos_->nestedIndex = result.callIndex;
				currentPos_->pos = input.current() - begin;
				std::size_t ruleIndex = currentPos_->ruleIndex;
				++currentPos_;
				currentPos_->ruleIndex = ruleInfos_[ruleIndex].nestedRuleIndexes[result.callIndex];
				currentPos_->pos = input.current() - begin;
				rule = pushRule( currentPos_->ruleIndex );
				input = RuleInput( input.current(), input.end() );
				continue;
			}
			else
			{
				if( currentPos_ == &positions_.front() )
				{
					if( result.code == RuleMatchCode::True )
					{
						const char* end_ = begin + input.position();
						callAction( currentPos_, rule, 0, begin, end_ );
						popRule( currentPos_->ruleIndex );
						currentPos_->pos = 0;
						if( begin == end_ )
							return { ParsingResult::Type::False, begin };
						return { ParsingResult::Type::True, end_ };
					}
					popRule( currentPos_->ruleIndex );
					currentPos_->pos = 0;
					return { ParsingResult::Type::False, begin };
				}
				if( result.code == RuleMatchCode::True )
				{
					std::size_t beginPos = ( currentPos_ - 1 )->pos;
					const char* begin_ = begin + beginPos;
					callAction( currentPos_, rule, beginPos, begin_, begin_ + input.position() );
				}
				RuleInput nestedInput = input;
				NestedRuleResult nestedResult( nestedInput );
				nestedResult.rule = rule;
				auto prevPos = currentPos_ - 1;
				auto prevRule = previousRule( currentPos_->ruleIndex, prevPos->ruleIndex );
				nestedResult.index = prevPos->nestedIndex;
				nestedResult.result = result.code == RuleMatchCode::True;
				input = RuleInput( begin + ( prevPos == &positions_.front() ? 0 : ( prevPos - 1 )->pos ), end, begin + prevPos->pos );
				result = prevRule->nestedResult( input, states_, nestedResult );
				popRule( currentPos_->ruleIndex );
				rule = prevRule;
				currentPos_ = prevPos;
				if( result.code == RuleMatchCode::TrueCanMore )
				{
					if( complete )
					{
						result.code = RuleMatchCode::True;
						goto ProcessResult;
					}
					currentPos_->pos = input.position();
					return { ParsingResult::Type::Incomplete, begin };
				}
				else if( result.code == RuleMatchCode::NotTrueYet )
				{
					if( complete )
					{
						result.code = RuleMatchCode::False;
						goto ProcessResult;
					}
					currentPos_->pos = input.position();
					return { ParsingResult::Type::Incomplete, begin };
				}
				goto ProcessResult;
			}
		}
		return { ParsingResult::Type::False, begin };
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	void Parser< Rule, ActionFunction, RulePositioning >::reset()
	{
		for( ; !isRuleStackEmpty(); --currentPos_ )
			popRule( currentPos_->ruleIndex );
		currentPos_->pos = 0;
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	template< std::size_t Id >
	inline void Parser< Rule, ActionFunction, RulePositioning >::initIndex( std::size_t& n )
	{
		indexes_[n++] = ( IndexType )Id;
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	template< std::size_t... Ids >
	inline void Parser< Rule, ActionFunction, RulePositioning >::initIndexes( std::size_t& n, std::index_sequence< Ids... > )
	{
		( initIndex< Ids >( n ), ... );
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	template< std::size_t Id >
	inline void Parser< Rule, ActionFunction, RulePositioning >::initRule( std::size_t& n )
	{
		using CRule = std::tuple_element_t< Id, UniqueRules >;
		ruleInfos_[Id].constructor = &detail::RuleConstructorStorage< CRule >::constructor;
		if constexpr( std::tuple_size_v< typename CRule::Rules > != 0 )
		{
			ruleInfos_[Id].nestedRuleIndexes = &indexes_[n];
			initIndexes( n, detail::index_sequence_for_part_in_whole_t< UniqueRules, typename CRule::Rules >{} );
		}
		else
			ruleInfos_[Id].nestedRuleIndexes = nullptr;
		if constexpr( UseActionFunction )
		{
			if constexpr( detail::tuple_element_index< CRule, typename ActionFunction::Rules >::value != ( std::size_t )-1 )
			{
				if constexpr( UseRulePosition )
					ruleInfos_[Id].func = &detail::ruleActionFunctionPos< CRule, ActionFunction >;
				else
					ruleInfos_[Id].func = &detail::ruleActionFunction< CRule, ActionFunction >;
			}
			else
				ruleInfos_[Id].func = nullptr;
		}
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	template< std::size_t... Ids >
	void Parser< Rule, ActionFunction, RulePositioning >::initRuleInfos( std::index_sequence< Ids... > )
	{
		std::size_t n = 0;
		( initRule< Ids >( n ), ... );
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	void Parser< Rule, ActionFunction, RulePositioning >::callAction( Position* pos, RuleBase* rule, std::size_t beginPos, const char* begin, const char* end )
	{
		if constexpr( UseActionFunction )
		{
			auto func = ruleInfos_[pos->ruleIndex].func;
			if( func )
			{
				if constexpr( UseRulePosition )
				{
					std::array< std::size_t, MaxDeep > rpos;
					auto it = rpos.end();
					while( pos != &positions_.front() )
					{
						--it;
						--pos;
						*it = pos->nestedIndex;
					}
					std::size_t n = rpos.end() - it;
					if( n )
						func( actionFunc_, std::span< const std::size_t >( &*it, n ), rule, beginPos, begin, end );
					else
						func( actionFunc_, std::span< const std::size_t >(), rule, beginPos, begin, end );
				}
				else
					func( actionFunc_, rule, beginPos, begin, end );
			}
		}
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	RuleBase* Parser< Rule, ActionFunction, RulePositioning >::pushRule( std::size_t index )
	{
		RuleBase* rule = ruleInfos_[index].constructor->construct( stackTop_ );
		stackTop_ += ruleInfos_[index].constructor->size();
		assert( stackTop_ <= &ruleStackData_.back() + 1 );
		return rule;
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	void Parser< Rule, ActionFunction, RulePositioning >::popRule( std::size_t index )
	{
		stackTop_ -= ruleInfos_[index].constructor->size();
		reinterpret_cast< RuleBase* >( stackTop_ )->~RuleBase();
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	RuleBase* Parser< Rule, ActionFunction, RulePositioning >::topRule( std::size_t index )
	{
		return reinterpret_cast< RuleBase* >( stackTop_ - ruleInfos_[index].constructor->size() );
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	RuleBase* Parser< Rule, ActionFunction, RulePositioning >::previousRule( std::size_t topIntex, std::size_t prevIndex )
	{
		return reinterpret_cast< RuleBase* >( stackTop_ - ruleInfos_[topIntex].constructor->size() - ruleInfos_[prevIndex].constructor->size() );
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	bool Parser< Rule, ActionFunction, RulePositioning >::isRuleStackEmpty() const noexcept
	{
		return stackTop_ == &ruleStackData_.front();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	template< char Begin, char End >
	class Range : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( *input.current() >= Begin && *input.current() <= End )
			{
				input.consume();
				return { RuleMatchCode::True };
			}
			return { RuleMatchCode::False };
		}
	};

	template< char Begin, char End >
	class NotRange : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( *input.current() <= Begin || *input.current() >= End )
			{
				input.consume();
				return { RuleMatchCode::True };
			}
			return { RuleMatchCode::False };
		}
	};

	template< typename Rule >
	class Plus : public RuleBase
	{
		bool one_ = false;

	public:
		using Rules = std::tuple< Rule >;

		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( !nestedResult.result )
			{
				if( !one_ )
					return { RuleMatchCode::False };
				return { RuleMatchCode::True };
			}
			one_ = true;
			input.setCurrent( nestedResult.input.current() );
			return { RuleMatchCode::CallNested, 0 };
		}
	};

	template< typename Rule >
	class Star : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule >;

		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( !nestedResult.result )
				return { RuleMatchCode::True };
			input.setCurrent( nestedResult.input.current() );
			return { RuleMatchCode::CallNested, 0 };
		}
	};

	template< typename Rule >
	class Opt : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
				input.setCurrent( nestedResult.input.current() );
			return { RuleMatchCode::True };
		}
	};

	template< typename Rule, typename... Others >
	class One : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule, Others... >;
		static constexpr std::size_t ChildrenCount = std::tuple_size_v< Rules >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
			{
				input.setCurrent( nestedResult.input.current() );
				return { RuleMatchCode::True };
			}
			else if( nestedResult.index != ( ChildrenCount - 1 ) )
				return { RuleMatchCode::CallNested, nestedResult.index + 1 };
			return { RuleMatchCode::False };
		}
	};

	template< typename Condition, typename Then, typename Else = detail::NaR >
	class If : public RuleBase
	{
	public:
		using Rules = std::conditional_t< std::is_same_v< Else, detail::NaR >, std::tuple< Condition, Then >, std::tuple< Condition, Then, Else > >;
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
					return { RuleMatchCode::CallNested, 1 };
				}
				else
				{
					if constexpr( std::is_same_v< Else, detail::NaR > )
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

	template< typename Rule >
	class Not : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
				return { RuleMatchCode::False };
			else
				return { RuleMatchCode::True };
		}
	};

	template< typename Rule, typename... Others >
	class Seq : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule, Others... >;
		static constexpr std::size_t ChildrenCount = std::tuple_size_v< Rules >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
			{
				input.setCurrent( nestedResult.input.current() );
				if( nestedResult.index == ( ChildrenCount - 1 ) )
					return { RuleMatchCode::True };
				return { RuleMatchCode::CallNested, nestedResult.index + 1 };
			}
			return { RuleMatchCode::False };
		}
	};

	template< char C >
	class Char : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( *input.current() == C )
			{
				input.consume();
				return { RuleMatchCode::True };
			}
			return { RuleMatchCode::False };
		}
	};

	template< char C, char... Cs >
	class Str : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			static std::array< const char, sizeof...( Cs ) + 1 > str{ C, Cs... };
			auto it = str.begin() + input.position();
			do
			{
				if( *input.current() != *it )
					return { RuleMatchCode::False };
				if( !input.consume() )
					return { RuleMatchCode::NotTrueYet };
				++it;
			} while( it != str.end() );
			return { RuleMatchCode::True };
		}
	};

	class Any : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			input.consume();
			return { RuleMatchCode::True };
		}
	};

	template< std::size_t N >
	class AnyN : public RuleBase
	{
	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.consume( N - input.position() ) )
				return { RuleMatchCode::True };
			return { RuleMatchCode::NotTrueYet };
		}
	};

	template< std::size_t Min, std::size_t Max, typename Rule >
	class RepMinMax : public RuleBase
	{
		static_assert( Min <= Max && Max != 0, "wrong parameters" );
		std::size_t n_ = 0;

	public:
		using Rules = std::tuple< Rule >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
			{
				input.setCurrent( nestedResult.input.current() );
				if( ++n_ == Max )
					return { RuleMatchCode::True };
				return { RuleMatchCode::CallNested, 0 };
			}
			if( n_ >= Min )
				return { RuleMatchCode::True };
			return { RuleMatchCode::False };
		}
	};

	template< std::size_t Min, std::size_t Max >
	class RepMinMax< Min, Max, Any > : public RuleBase
	{
		static_assert( Min <= Max && Max != 0, "wrong parameters" );
		std::size_t n_ = 0;

	public:
		RuleResult match( RuleInputRef input, void* states ) override
		{
			if( input.consume( Max - input.position() ) )
				return { RuleMatchCode::True };
			if( input.position() >= Min )
				return { RuleMatchCode::True };
			return { RuleMatchCode::NotTrueYet };
		}
	};

	template< std::size_t Min, typename Rule >
	class RepMin : public RepMinMax< Min, ( std::size_t )-1, Rule >
	{
	};

	template< std::size_t Max, typename Rule >
	class RepMax : public RepMinMax< 0, Max, Rule >
	{
	};

	template< typename Rule >
	class Rep : public RepMinMax< 0, ( std::size_t )-1, Rule >
	{
	};

	template< typename Rule, std::size_t Max >
	class UntilMax : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule >;
		RuleResult match( RuleInputRef input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
				return { RuleMatchCode::True };
			if( input.position() < Max )
			{
				if( input.consume() )
					return { RuleMatchCode::CallNested, 0 };
				return { RuleMatchCode::NotTrueYet };
			}
			return { RuleMatchCode::False };
		}
	};

	template< typename Rule >
	class Until : public UntilMax< Rule, ( std::size_t )-1 >
	{
	};

	class Digit : public Range< '0', '9' >
	{
	};

	class PositiveInteger : public Plus< Digit >
	{
	};

	class Integer : public Seq< Opt< One< Char< '+' >, Char< '-' > > >, Plus< Digit > >
	{
	};
} // namespace psm