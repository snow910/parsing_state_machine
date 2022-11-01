#pragma once

#include <array>
#include <span>
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
		inline RuleInput( const char* begin, const char* end, std::size_t pos ) noexcept
		    : begin_( begin ), end_( end ), current_( begin + pos )
		{
			if( current_ > end_ )
				current_ = end_;
		}
		inline RuleInput( const RuleInput& ) = default;
		inline RuleInput& operator=( const RuleInput& ) = default;

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
		inline void consume( std::size_t size )
		{
			current_ += size;
			if( current_ > end_ )
				current_ = end_;
		}
		inline void discard() noexcept { current_ = begin_; }
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

		virtual RuleResult match( RuleInput& input, void* states )
		{
			return { RuleMatchCode::False };
		}
		virtual RuleResult nestedResult( RuleInput& input, void* states, const NestedRuleResult& nestedResult )
		{
			return { RuleMatchCode::False };
		}
		virtual void reset()
		{
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

		template< std::size_t Deep, typename Rules >
		struct _nested_rules_info;

		template< std::size_t Deep, typename... Rules_ >
		struct _nested_rules_info< Deep, std::tuple< Rules_... > >
		{
			static constexpr std::size_t max_deep = max< std::size_t, _nested_rules_info< Deep + 1, typename Rules_::Rules >::max_deep... >::value;
			using unique_rules = unique_tuple_cat_t< typename _nested_rules_info< Deep + 1, typename Rules_::Rules >::unique_rules..., unique_types_t< std::tuple< Rules_... > > >;
		};

		template< std::size_t Deep >
		struct _nested_rules_info< Deep, std::tuple<> >
		{
			static constexpr std::size_t max_deep = Deep;
			using unique_rules = std::tuple<>;
		};

		template< typename Rule >
		struct nested_rules_info : _nested_rules_info< 0, typename Rule::Rules >
		{
		};

		template< typename Rules >
		struct dependent_rule_count;

		template< typename... Rules_ >
		struct dependent_rule_count< std::tuple< Rules_... > >
		    : std::integral_constant< std::size_t, integer_sequence_sum< std::index_sequence< std::tuple_size_v< typename Rules_::Rules >... > >::value >
		{
		};

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
	} // namespace detail

	enum class ParsingResultType
	{
		True,
		False,
		Incomplete
	};

	class ParserBase
	{
	public:
		virtual ~ParserBase() {}

		virtual std::pair< ParsingResultType, const char* > parse( const char* begin, const char* end, bool complete = true ) = 0;
		virtual void reset() = 0;

		template< std::size_t N >
		inline std::pair< ParsingResultType, const char* > parseString( const char ( &str )[N] ) { return parse( str, str + N - 1 ); }
	};

	// ActionFunction is a functional object type that contains Rules (a std::tuple alias) of interesting rules
	template< typename Rule, typename ActionFunction = detail::Nothing, bool RulePositioning = false >
	class Parser : public ParserBase
	{
	public:
		using UniqueRules = detail::push_type_front_t< typename detail::nested_rules_info< Rule >::unique_rules, Rule >;
		static constexpr std::size_t MaxDeep = detail::nested_rules_info< Rule >::max_deep;
		static constexpr bool UseActionFunction = std::tuple_size_v< typename ActionFunction::Rules > != 0;
		static constexpr bool UseRulePosition = UseActionFunction && RulePositioning;

		Parser()
		{
			initRuleInfos( std::make_index_sequence< std::tuple_size_v< UniqueRules > >{} );
			current_ = &ruleInfos_.front();
		}
		Parser( ActionFunction func ) : actionFunc_( std::move( func ) )
		{
			initRuleInfos( std::make_index_sequence< std::tuple_size_v< UniqueRules > >{} );
			current_ = &ruleInfos_.front();
		}

		inline void setActionFunction( ActionFunction func )
		{
			actionFunc_ = std::move( func );
		}

		inline void setStates( void* states )
		{
			states_ = states;
		}

		std::pair< ParsingResultType, const char* > parse( const char* begin, const char* end, bool complete ) override
		{
			if( begin == end && !complete )
				return { ParsingResultType::Incomplete, begin };

			RuleInput input( begin + current_->beginPos, end, current_->pos );
			RuleResult result;
			while( true )
			{
				if( input.size() )
				{
					result = current_->rule->match( input, states_ );
					if( result.code == RuleMatchCode::TrueCanMore )
					{
						if( !complete )
						{
							current_->pos = input.position();
							return { ParsingResultType::Incomplete, begin };
						}
						result.code = RuleMatchCode::True;
					}
					else if( result.code == RuleMatchCode::NotTrueYet )
					{
						if( !complete )
						{
							current_->pos = input.position();
							return { ParsingResultType::Incomplete, begin };
						}
						result.code = RuleMatchCode::False;
					}
				}
				else if( complete )
					result.code = RuleMatchCode::False;
				else
				{
					current_->pos = input.position();
					return { ParsingResultType::Incomplete, begin };
				}

			ProcessResult:
				if( result.code == RuleMatchCode::CallNested )
				{
					current_->nextIndex = result.callIndex;
					auto nextRuleIndex = current_->nestedRuleIndexes[result.callIndex];
					ruleInfos_[nextRuleIndex].prevIndex = ( std::size_t )( current_ - &ruleInfos_.front() );
					current_->pos = input.position();
					current_ = &ruleInfos_[nextRuleIndex];
					current_->beginPos = ( std::size_t )( input.current() - begin );
					current_->pos = input.position();
					current_->rule->reset();
					input = RuleInput( input.current(), input.end() );
					continue;
				}
				else
				{
					if( current_ == &ruleInfos_.front() )
					{
						current_->rule->reset();
						current_->pos = 0;
						if( result.code == RuleMatchCode::True )
						{
							const char* end_ = begin + input.position();
							callAction( 0, begin, end_ );
							return { ParsingResultType::True, end_ };
						}
						return { ParsingResultType::False, begin };
					}
					if( result.code == RuleMatchCode::True )
					{
						const char* begin_ = begin + current_->beginPos;
						callAction( current_->beginPos, begin_, begin_ + input.position() );
					}
					RuleInput nestedInput = input;
					NestedRuleResult nestedResult( nestedInput );
					nestedResult.rule = current_->rule;
					current_ = &ruleInfos_[current_->prevIndex];
					input = RuleInput( begin + current_->beginPos, end, current_->pos );
					nestedResult.index = current_->nextIndex;
					nestedResult.result = result.code == RuleMatchCode::True;
					result = current_->rule->nestedResult( input, states_, nestedResult );
					if( result.code == RuleMatchCode::TrueCanMore )
					{
						if( complete )
						{
							result.code = RuleMatchCode::True;
							goto ProcessResult;
						}
						current_->pos = input.position();
						return { ParsingResultType::Incomplete, begin };
					}
					else if( result.code == RuleMatchCode::NotTrueYet )
					{
						if( complete )
						{
							result.code = RuleMatchCode::False;
							goto ProcessResult;
						}
						current_->pos = input.position();
						return { ParsingResultType::Incomplete, begin };
					}
					goto ProcessResult;
				}
			}
			return { ParsingResultType::False, begin };
		}

		void reset() override
		{
			current_ = &ruleInfos_.front();
			current_->rule->reset();
			current_->pos = 0;
		}

	private:
		static constexpr std::size_t IndexArraySize = detail::dependent_rule_count< UniqueRules >::value;
		using IndexType = std::conditional_t< IndexArraySize <= 256, uint8_t, uint16_t >;

		template< std::size_t Id >
		inline void initIndex( std::size_t& n )
		{
			indexes_[n++] = ( IndexType )Id;
		}

		template< std::size_t... Ids >
		inline void initIndexes( std::size_t& n, std::index_sequence< Ids... > )
		{
			( initIndex< Ids >( n ), ... );
		}

		template< std::size_t Id >
		inline void initRule( std::size_t& n )
		{
			using CRule = std::tuple_element_t< Id, UniqueRules >;
			ruleInfos_[Id].rule = &std::get< Id >( rules_ );
			if constexpr( std::tuple_size_v< typename CRule::Rules > != 0 )
			{
				ruleInfos_[Id].nestedRuleIndexes = &indexes_[n];
				initIndexes( n, detail::index_sequence_for_part_in_whole_t< UniqueRules, typename CRule::Rules >{} );
			}
			if constexpr( UseActionFunction && detail::tuple_element_index< CRule, ActionFunction::Rules >::value != ( std::size_t )-1 )
			{
				if constexpr( UseRulePosition )
					ruleInfos_[Id].func = &detail::ruleActionFunctionPos< CRule, ActionFunction >;
				else
					ruleInfos_[Id].func = &detail::ruleActionFunction< CRule, ActionFunction >;
			}
		}

		template< std::size_t... Ids >
		void initRuleInfos( std::index_sequence< Ids... > )
		{
			std::size_t n = 0;
			( initRule< Ids >( n ), ... );
		}

		void callAction( std::size_t beginPos, const char* begin, const char* end )
		{
			if constexpr( UseActionFunction )
			{
				if( current_->func )
				{
					if constexpr( UseRulePosition )
					{
						std::array< std::size_t, MaxDeep > pos;
						auto it = pos.end();
						auto info = current_;
						while( info != &ruleInfos_.front() )
						{
							--it;
							info = &ruleInfos_[info->prevIndex];
							*it = info->nextIndex;
						}
						std::size_t n = pos.end() - it;
						if( n )
							current_->func( actionFunc_, std::span< const std::size_t >( &*it, n ), current_->rule, beginPos, begin, end );
						else
							current_->func( actionFunc_, std::span< const std::size_t >(), current_->rule, beginPos, begin, end );
					}
					else
						current_->func( actionFunc_, current_->rule, beginPos, begin, end );
				}
			}
		}

	private:
		using _RuleActionFunction = void ( * )( ActionFunction&, const RuleBase*, std::size_t, const char*, const char* );
		using _RuleActionFunctionPos = void ( * )( ActionFunction&, const std::span< const std::size_t >&, const RuleBase*, std::size_t, const char*, const char* );
		using RuleActionFunction = std::conditional_t< UseRulePosition, _RuleActionFunctionPos, _RuleActionFunction >;
		ActionFunction actionFunc_;
		void* states_ = nullptr;
		UniqueRules rules_;
		struct _RuleInfoV0
		{
			RuleBase* rule;
			IndexType* nestedRuleIndexes;
			std::size_t nextIndex = 0;
			std::size_t prevIndex = 0;
			std::size_t beginPos = 0;
			std::size_t pos = 0;
			RuleActionFunction func = nullptr;
		};
		struct _RuleInfoV1
		{
			RuleBase* rule;
			IndexType* nestedRuleIndexes;
			std::size_t nextIndex = 0;
			std::size_t prevIndex = 0;
			std::size_t beginPos = 0;
			std::size_t pos = 0;
		};
		using RuleInfo = std::conditional_t< UseActionFunction, _RuleInfoV0, _RuleInfoV1 >;
		std::array< RuleInfo, std::tuple_size_v< UniqueRules > > ruleInfos_;
		std::array< IndexType, IndexArraySize > indexes_;
		RuleInfo* current_;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////////

	template< char Begin_, char End_ >
	class Range : public RuleBase
	{
	public:
		RuleResult match( RuleInput& input, void* states ) override
		{
			if( *input.current() >= Begin_ && *input.current() <= End_ )
			{
				input.consume( 1 );
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

		RuleResult match( RuleInput& input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInput& input, void* states, const NestedRuleResult& nestedResult ) override
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
		void reset() override
		{
			one_ = false;
		}
	};

	template< typename Rule >
	class Star : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule >;

		RuleResult match( RuleInput& input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInput& input, void* states, const NestedRuleResult& nestedResult ) override
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
		RuleResult match( RuleInput& input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInput& input, void* states, const NestedRuleResult& nestedResult ) override
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
		RuleResult match( RuleInput& input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInput& input, void* states, const NestedRuleResult& nestedResult ) override
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

	template< typename Rule, typename... Others >
	class Seq : public RuleBase
	{
	public:
		using Rules = std::tuple< Rule, Others... >;
		static constexpr std::size_t ChildrenCount = std::tuple_size_v< Rules >;
		RuleResult match( RuleInput& input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInput& input, void* states, const NestedRuleResult& nestedResult ) override
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
		RuleResult match( RuleInput& input, void* states ) override
		{
			if( *input.current() == C )
			{
				input.consume( 1 );
				return { RuleMatchCode::True };
			}
			return { RuleMatchCode::False };
		}
	};

	template< std::size_t N >
	class Whatever : public RuleBase
	{
		std::size_t n_ = N;

	public:
		RuleResult match( RuleInput& input, void* states ) override
		{
			auto n = input.size();
			if( n > n_ )
			{
				input.consume( n_ );
				return { RuleMatchCode::True };
			}
			else
			{
				input.consume( n );
				n_ -= n;
				return { RuleMatchCode::NotTrueYet };
			}
		}
		void reset() override
		{
			n_ = N;
		}
	};

	template< std::size_t Min, std::size_t Max, typename Rule >
	class RepMinMax : public RuleBase
	{
		static_assert( Min <= Max && Max != 0, "wrong parameters" );
		std::size_t n_ = 0;

	public:
		using Rules = std::tuple< Rule >;
		RuleResult match( RuleInput& input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInput& input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
			{
				if( ++n_ == Max )
					return { RuleMatchCode::True };
				return { RuleMatchCode::CallNested, 0 };
			}
			if( n_ >= Min )
				return { RuleMatchCode::True };
			return { RuleMatchCode::False };
		}
		void reset() override
		{
			n_ = 0;
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
		RuleResult match( RuleInput& input, void* states ) override
		{
			return { RuleMatchCode::CallNested, 0 };
		}
		RuleResult nestedResult( RuleInput& input, void* states, const NestedRuleResult& nestedResult ) override
		{
			if( nestedResult.result )
				return { RuleMatchCode::True };
			if( input.position() < Max )
				return { RuleMatchCode::CallNested, 0 };
			return { RuleMatchCode::False };
		}
	};

	template< typename Rule, std::size_t Max >
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