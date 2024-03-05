#pragma once

#include "Rule.h"
#include <array>
#include <assert.h>
#include <span>
#include <type_traits>

// Parsing state machine
namespace psm
{
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

		template< typename Type, std::size_t... Vs >
		constexpr std::size_t max_v = max< Type, Vs... >::value;

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

		template< typename T, typename Tp >
		constexpr std::size_t tuple_element_index_v = tuple_element_index< T, Tp >::value;

		template< typename Whole, typename Part >
		struct index_sequence_for_part_in_whole;

		template< typename Whole, typename... Part >
		struct index_sequence_for_part_in_whole< Whole, std::tuple< Part... > >
		{
			using type = std::index_sequence< tuple_element_index_v< Part, Whole >... >;
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
		    : std::conditional_t< tuple_element_index_v< Other, Unique > == ( std::size_t )-1,
					  push_unique_types_back< push_type_back_t< Unique, Other >, std::tuple< Others... > >,
					  push_unique_types_back< Unique, std::tuple< Others... > > >
		{
		};

		template< typename Unique, typename Other >
		struct push_unique_types_back< Unique, std::tuple< Other > >
		{
			using type = std::conditional_t< tuple_element_index_v< Other, Unique > == ( std::size_t )-1,
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

		template< typename Tuple1, typename Tuple2 >
		struct tuple_cat2;

		template< typename... Ts1, typename... Ts2 >
		struct tuple_cat2< std::tuple< Ts1... >, std::tuple< Ts2... > >
		{
			using type = std::tuple< Ts1..., Ts2... >;
		};

		template< typename Tuple1, typename Tuple2 >
		using tuple_cat2_t = typename tuple_cat2< Tuple1, Tuple2 >::type;

		template< typename... Tuples >
		struct tuple_cat;

		template< typename Tuple1, typename Tuple2, typename... Others >
		struct tuple_cat< Tuple1, Tuple2, Others... > : tuple_cat2< typename tuple_cat2< Tuple1, Tuple2 >::type, typename tuple_cat< Others... >::type >
		{
		};

		template< typename Tuple1, typename Tuple2 >
		struct tuple_cat< Tuple1, Tuple2 > : tuple_cat2< Tuple1, Tuple2 >
		{
		};

		template< typename Tuple >
		struct tuple_cat< Tuple >
		{
			using type = Tuple;
		};

		template< typename... Tuples >
		using tuple_cat_t = tuple_cat< Tuples... >::type;

		//===========================================================================================================================================

		template< typename... RuleInfos >
		struct unite_rules_info
		{
			static constexpr std::size_t max_deep_mpl = max_v< std::size_t, RuleInfos::max_deep_mpl... >;
			static constexpr std::size_t max_deep_adt = max_v< std::size_t, RuleInfos::max_deep_adt... >;
			static constexpr std::size_t stack_size_mpl = max_v< std::size_t, RuleInfos::stack_size_mpl... >;
			static constexpr std::size_t stack_size_adt = max_v< std::size_t, RuleInfos::stack_size_adt... >;
			using unique_rules = unique_tuple_cat_t< typename RuleInfos::unique_rules... >;
			using generators = tuple_cat_t< typename RuleInfos::generators... >;
		};

		template<>
		struct unite_rules_info<>
		{
			static constexpr std::size_t max_deep_mpl = 0;
			static constexpr std::size_t max_deep_adt = 0;
			static constexpr std::size_t stack_size_mpl = 0;
			static constexpr std::size_t stack_size_adt = 0;
			using unique_rules = std::tuple<>;
			using generators = std::tuple<>;
		};

		template< typename Rule >
		struct _rule_info;

		template< typename Rules >
		struct _rules_info;

		template< typename... Rules >
		struct _rules_info< std::tuple< Rules... > > : unite_rules_info< _rule_info< Rules >... >
		{
		};

		template< typename Rule >
		struct _rule_info
		{
			static constexpr std::size_t max_deep_mpl = _rules_info< typename Rule::Rules >::max_deep_mpl + 1;
			static constexpr std::size_t max_deep_adt = _rules_info< typename Rule::Rules >::max_deep_adt;
			static constexpr std::size_t stack_size_mpl = _rules_info< typename Rule::Rules >::stack_size_mpl + sizeof( Rule );
			static constexpr std::size_t stack_size_adt = _rules_info< typename Rule::Rules >::stack_size_adt;
			using unique_rules = unique_tuple_cat_t< typename _rules_info< typename Rule::Rules >::unique_rules, std::tuple< Rule > >;
			using generators = typename _rules_info< typename Rule::Rules >::generators;
		};

		template< typename Tag, std::size_t MaxCount, typename Rule >
		struct _rule_info< GenA< Tag, MaxCount, Rule > >
		{
			static constexpr std::size_t max_deep_mpl = 0;
			static constexpr std::size_t max_deep_adt = _rule_info< Rule >::max_deep_adt + _rule_info< Rule >::max_deep_mpl * MaxCount;
			static constexpr std::size_t stack_size_mpl = 0;
			static constexpr std::size_t stack_size_adt = _rule_info< Rule >::stack_size_adt + _rule_info< Rule >::stack_size_mpl * MaxCount;
			using unique_rules = _rule_info< Rule >::unique_rules;
			using generators = push_type_back_t< typename _rule_info< Rule >::generators, GenA< Tag, MaxCount, Rule > >;
		};

		template< typename Tag, std::size_t MaxCount, typename Rule >
		struct _rule_info< GenM< Tag, MaxCount, Rule > >
		{
			static constexpr std::size_t max_deep_mpl = _rule_info< Rule >::max_deep_mpl * MaxCount;
			static constexpr std::size_t max_deep_adt = _rule_info< Rule >::max_deep_adt;
			static constexpr std::size_t stack_size_mpl = _rule_info< Rule >::stack_size_mpl * MaxCount;
			static constexpr std::size_t stack_size_adt = _rule_info< Rule >::stack_size_adt;
			using unique_rules = _rule_info< Rule >::unique_rules;
			using generators = push_type_back_t< typename _rule_info< Rule >::generators, GenM< Tag, MaxCount, Rule > >;
		};

		template< typename Tag >
		struct _rule_info< Ref< Tag > >
		{
			static constexpr std::size_t max_deep_mpl = 0;
			static constexpr std::size_t max_deep_adt = 0;
			static constexpr std::size_t stack_size_mpl = 0;
			static constexpr std::size_t stack_size_adt = 0;
			using unique_rules = std::tuple<>;
			using generators = std::tuple<>;
		};

		template< typename Rule >
		struct rule_info
		{
			static constexpr std::size_t max_deep = _rule_info< Rule >::max_deep_mpl + _rule_info< Rule >::max_deep_adt;
			static constexpr std::size_t stack_size = _rule_info< Rule >::stack_size_mpl + _rule_info< Rule >::stack_size_adt;
			using unique_rules = typename _rule_info< Rule >::unique_rules;
			using generators = typename _rule_info< Rule >::generators;
		};

		template< typename Rules >
		struct dependent_rule_count;

		template< typename... Rules_ >
		struct dependent_rule_count< std::tuple< Rules_... > >
		    : std::integral_constant< std::size_t, integer_sequence_sum< std::index_sequence< std::tuple_size_v< typename Rules_::Rules >... > >::value >
		{
		};

		template< typename Tag, typename... Generators >
		struct _find_generator;

		template< typename Tag_, typename Generator, typename... Generators >
		struct _find_generator< Tag_, Generator, Generators... >
		{
			using type = std::conditional_t< std::is_same_v< Tag_, typename Generator::Tag >, Generator, typename _find_generator< Tag_, Generators... >::type >;
		};

		template< typename Tag_ >
		struct _find_generator< Tag_ >
		{
			using type = void;
		};

		template< typename Tag, typename Generators >
		struct find_generator;

		template< typename Tag, typename... Generators >
		struct find_generator< Tag, std::tuple< Generators... > > : _find_generator< Tag, Generators... >
		{
		};

		template< typename Tag, typename Generators >
		using find_generator_t = typename find_generator< Tag, Generators >::type;

		template< typename Generators, typename Rule >
		struct _gen_ref_substitution
		{
			using type = Rule;
		};

		template< typename Generators, typename Tag, std::size_t MaxCount, typename Rule >
		struct _gen_ref_substitution< Generators, GenA< Tag, MaxCount, Rule > >
		{
			using type = typename _gen_ref_substitution< Generators, Rule >::type;
		};

		template< typename Generators, typename Tag, std::size_t MaxCount, typename Rule >
		struct _gen_ref_substitution< Generators, GenM< Tag, MaxCount, Rule > >
		{
			using type = typename _gen_ref_substitution< Generators, Rule >::type;
		};

		template< typename Generators, typename Tag >
		struct _gen_ref_substitution< Generators, Ref< Tag > >
		{
			using generator = find_generator_t< Tag, Generators >;
			static_assert( !std::is_same_v< generator, void >, "tag not found" );
			using type = typename _gen_ref_substitution< Generators, typename generator::Rule >::type;
		};

		template< typename Generators, typename Rules >
		struct gen_ref_substitution;

		template< typename Generators, typename... Rules >
		struct gen_ref_substitution< Generators, std::tuple< Rules... > >
		{
			using type = std::tuple< typename _gen_ref_substitution< Generators, Rules >::type... >;
		};

		template< typename Rules >
		struct gen_ref_substitution< std::tuple<>, Rules >
		{
			using type = Rules;
		};

		template< typename Generators, typename Rules >
		using gen_ref_substitution_t = typename gen_ref_substitution< Generators, Rules >::type;

		template< typename Rule >
		struct is_quiet_rule : std::false_type
		{
		};

		template< typename Rule >
		struct is_quiet_rule< Quiet< Rule > > : std::true_type
		{
		};

		struct RuleConstructorBase
		{
			virtual RuleBase* construct( void* mem ) const noexcept = 0;
			virtual std::size_t size() const noexcept = 0;
			virtual bool isQuiet() const noexcept = 0;
			virtual std::string_view name() const noexcept = 0;
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
			bool isQuiet() const noexcept override
			{
				return is_quiet_rule< Rule >::value;
			}
			std::string_view name() const noexcept override
			{
				return typeid( Rule ).name();
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

		template< typename Type, typename = void >
		struct is_tracing_enabled : std::false_type
		{
		};

		template< typename Type >
		struct is_tracing_enabled< Type, std::enable_if_t< Type::PsmTracing == true > > : std::true_type
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
		using RuleTypeInfo = detail::rule_info< Rule >;

	public:
		using UniqueRules = typename RuleTypeInfo::unique_rules;
		using Generators = typename RuleTypeInfo::generators;
		static constexpr std::size_t MaxDeep = RuleTypeInfo::max_deep - 1;
		static constexpr bool UseActionFunction = std::tuple_size_v< typename ActionFunction::Rules > != 0;
		static constexpr bool UseRulePosition = UseActionFunction && RulePositioning;
		static constexpr bool Tracing = detail::is_tracing_enabled< ActionFunction >::value;

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
		void trace( Position* pos, RuleInputRef input, RuleResult result );
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
		std::array< Position, RuleTypeInfo::max_deep > positions_;
		std::array< uint8_t, RuleTypeInfo::stack_size > ruleStackData_;
		Position* currentPos_;
		uint8_t* stackTop_;
		std::size_t quietCounter_ = 0;
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
			result = rule->match( input, states_ );
			if constexpr( Tracing )
				trace( currentPos_, input, result );
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

		ProcessResult:
			if( result.code == RuleMatchCode::CallNested )
			{
				if constexpr( std::tuple_size_v< Generators > != 0 )
				{
					if( currentPos_ == &positions_.back() )
					{
						reset();
						return { ParsingResult::Type::False, begin };
					}
				}
				currentPos_->nestedIndex = result.callIndex;
				currentPos_->pos = input.current() - begin;
				std::size_t ruleIndex = currentPos_->ruleIndex;
				++currentPos_;
				currentPos_->ruleIndex = ruleInfos_[ruleIndex].nestedRuleIndexes[result.callIndex];
				currentPos_->pos = input.current() - begin;
				rule = pushRule( currentPos_->ruleIndex );
				input = RuleInput( input.current(), input.end() );
				if constexpr( std::tuple_size_v< Generators > != 0 )
				{
					if( rule == nullptr )
					{
						--currentPos_;
						reset();
						return { ParsingResult::Type::False, begin };
					}
				}
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
				if constexpr( Tracing )
					trace( currentPos_, input, result );
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
			initIndexes( n, detail::index_sequence_for_part_in_whole_t< UniqueRules, detail::gen_ref_substitution_t< Generators, typename CRule::Rules > >{} );
		}
		else
			ruleInfos_[Id].nestedRuleIndexes = nullptr;
		if constexpr( UseActionFunction )
		{
			if constexpr( detail::tuple_element_index_v< CRule, typename ActionFunction::Rules > != ( std::size_t )-1 )
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
			if( quietCounter_ )
				return;
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
	void Parser< Rule, ActionFunction, RulePositioning >::trace( Position* pos, RuleInputRef input, RuleResult result )
	{
		actionFunc_.trace( pos - &positions_.front(), input, result, ruleInfos_[pos->ruleIndex].constructor->name() );
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	RuleBase* Parser< Rule, ActionFunction, RulePositioning >::pushRule( std::size_t index )
	{
		if constexpr( std::tuple_size_v< Generators > != 0 )
			if( stackTop_ - &ruleStackData_.front() + ruleInfos_[index].constructor->size() > ruleStackData_.size() )
				return nullptr;
		RuleBase* rule = ruleInfos_[index].constructor->construct( stackTop_ );
		stackTop_ += ruleInfos_[index].constructor->size();
		if( ruleInfos_[index].constructor->isQuiet() )
			++quietCounter_;
		assert( stackTop_ <= &ruleStackData_.back() + 1 );
		return rule;
	}

	template< typename Rule, typename ActionFunction, bool RulePositioning >
	void Parser< Rule, ActionFunction, RulePositioning >::popRule( std::size_t index )
	{
		stackTop_ -= ruleInfos_[index].constructor->size();
		reinterpret_cast< RuleBase* >( stackTop_ )->~RuleBase();
		if( ruleInfos_[index].constructor->isQuiet() )
			--quietCounter_;
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
} // namespace psm