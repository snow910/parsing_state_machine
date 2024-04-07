// Copyright (c) 2024 snow910
// Distributed under the Boost Software License, Version 1.0.
// See accompanying file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt

#pragma once

#include "Rule.h"
#include <array>
#include <compare>
#include <span>
#include <type_traits>
#include <typeinfo>

// Parsing state machine
namespace psm
{
	namespace detail
	{
		//===========================================================================================================================================
		// meta functions

		template< typename Type, std::size_t... Vs >
		struct sum
		{
			static constexpr Type value = ( Vs + ... );
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

		template< typename Seq0, typename Seq1 >
		struct integer_sequence_cat2;

		template< typename Type, Type... Vs0, Type... Vs1 >
		struct integer_sequence_cat2< std::integer_sequence< Type, Vs0... >, std::integer_sequence< Type, Vs1... > >
		{
			using type = std::integer_sequence< Type, Vs0..., Vs1... >;
		};

		template< typename Seq0, typename Seq1 >
		using integer_sequence_cat2_t = typename integer_sequence_cat2< Seq0, Seq1 >::type;

		template< std::size_t TupleSize, typename T, typename Tp >
		struct tuple_element_index;

		template< std::size_t TupleSize, typename T, typename... Ts >
		struct tuple_element_index< TupleSize, T, std::tuple< T, Ts... > >
		{
			static constexpr std::size_t value = 0;
		};

		template< std::size_t TupleSize, typename T, typename T0, typename... Ts >
		struct tuple_element_index< TupleSize, T, std::tuple< T0, Ts... > >
		{
			static constexpr std::size_t value = 1 + tuple_element_index< TupleSize, T, std::tuple< Ts... > >::value;
		};

		template< std::size_t TupleSize, typename T >
		struct tuple_element_index< TupleSize, T, std::tuple<> >
		{
			static constexpr std::size_t value = ( std::size_t )-1 - TupleSize;
		};

		template< typename T, typename Tuple >
		constexpr std::size_t tuple_element_index_v = tuple_element_index< std::tuple_size_v< Tuple >, T, Tuple >::value;

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

		template< typename... Tuples >
		using tuple_cat_t = decltype( std::tuple_cat( std::declval< Tuples >()... ) );

		template< typename T, typename... Ts >
		struct unique : std::type_identity< T >
		{
		};

		template< typename... Ts, typename U, typename... Us >
		struct unique< std::tuple< Ts... >, U, Us... >
		    : std::conditional_t< ( std::is_same_v< U, Ts > || ... ), unique< std::tuple< Ts... >, Us... >, unique< std::tuple< Ts..., U >, Us... > >
		{
		};

		template< typename... Ts >
		struct unique_types_tuple;

		template< typename... Ts >
		struct unique_types_tuple< std::tuple< Ts... > > : unique< std::tuple<>, Ts... >
		{
		};

		template< typename... Tuples >
		using unique_tuple_cat_t = typename unique_types_tuple< tuple_cat_t< Tuples... > >::type;

		//===========================================================================================================================================
		// rule info

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
			using unique_rules = typename _rule_info< Rule >::unique_rules;
			using generators = push_type_back_t< typename _rule_info< Rule >::generators, GenA< Tag, MaxCount, Rule > >;
		};

		template< typename Tag, std::size_t MaxCount, typename Rule >
		struct _rule_info< GenM< Tag, MaxCount, Rule > >
		{
			static constexpr std::size_t max_deep_mpl = _rule_info< Rule >::max_deep_mpl * MaxCount;
			static constexpr std::size_t max_deep_adt = _rule_info< Rule >::max_deep_adt;
			static constexpr std::size_t stack_size_mpl = _rule_info< Rule >::stack_size_mpl * MaxCount;
			static constexpr std::size_t stack_size_adt = _rule_info< Rule >::stack_size_adt;
			using unique_rules = typename _rule_info< Rule >::unique_rules;
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

		template< typename... Rules >
		struct gen_ref_substitution< std::tuple<>, std::tuple< Rules... > >
		{
			using type = std::tuple< Rules... >;
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

		template< typename UniqueRules, typename Generators, typename... Rules >
		struct _subrule_index_sequence
		{
		};

		template< typename UniqueRules, typename Generators, typename Rule, typename... Others >
		struct _subrule_index_sequence< UniqueRules, Generators, Rule, Others... >
		{
			using _current = index_sequence_for_part_in_whole_t< UniqueRules, detail::gen_ref_substitution_t< Generators, typename Rule::Rules > >;
			using type = integer_sequence_cat2_t< _current, typename _subrule_index_sequence< UniqueRules, Generators, Others... >::type >;
		};

		template< typename UniqueRules, typename Generators, typename Rule >
		struct _subrule_index_sequence< UniqueRules, Generators, Rule >
		{
			using type = index_sequence_for_part_in_whole_t< UniqueRules, detail::gen_ref_substitution_t< Generators, typename Rule::Rules > >;
		};

		template< typename UniqueRules, typename Generators >
		struct subrule_index_sequence;

		template< typename... UniqueRules, typename Generators >
		struct subrule_index_sequence< std::tuple< UniqueRules... >, Generators > : _subrule_index_sequence< std::tuple< UniqueRules... >, Generators, UniqueRules... >
		{
		};

		template< typename UniqueRules, typename Generators >
		using subrule_index_sequence_t = typename subrule_index_sequence< UniqueRules, Generators >::type;

		template< size_t Offset, typename Ids, typename... Rules >
		struct _subrule_index_offset_sequence
		{
		};

		template< std::size_t Offset, std::size_t... Ids, typename Rule, typename... Others >
		struct _subrule_index_offset_sequence< Offset, std::index_sequence< Ids... >, Rule, Others... >
		    : _subrule_index_offset_sequence< Offset + std::tuple_size_v< typename Rule::Rules >, std::index_sequence< Ids..., Offset >, Others... >
		{
		};

		template< size_t Offset, std::size_t... Ids, typename Rule >
		struct _subrule_index_offset_sequence< Offset, std::index_sequence< Ids... >, Rule >
		{
			using type = std::index_sequence< Ids..., Offset >;
		};

		template< typename Rules >
		struct subrule_index_offset_sequence;

		template< typename... Rules >
		struct subrule_index_offset_sequence< std::tuple< Rules... > > : _subrule_index_offset_sequence< 0, std::index_sequence<>, Rules... >
		{
		};

		template< typename Rules >
		using subrule_index_offset_sequence_t = typename subrule_index_offset_sequence< Rules >::type;

		//===========================================================================================================================================
		// rule constraction

		struct RuleConstructorBase
		{
			virtual RuleBase* construct( void* mem ) const noexcept = 0;
			virtual std::size_t size() const noexcept = 0;
			virtual bool isQuiet() const noexcept = 0;
		};

		struct TracingRuleConstructorBase : RuleConstructorBase
		{
			virtual std::string_view name() const noexcept = 0;
		};

		template< typename Rule, bool Tracing >
		struct RuleConstructor;

		template< typename Rule >
		struct RuleConstructor< Rule, false > : RuleConstructorBase
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
		};

		template< typename Rule >
		struct RuleConstructor< Rule, true > : TracingRuleConstructorBase
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

		template< typename Rule, bool Tracing >
		struct RuleConstructorStorage
		{
			static constexpr RuleConstructor< Rule, Tracing > constructor{};
		};

		//===========================================================================================================================================
		// rule action

		template< typename Rule, typename Action >
		bool ruleActionFunction( Action& action, const RuleBase* rule, std::size_t pos, const std::string_view& string )
		{
			using ReturnType = std::invoke_result_t< Action, const Rule&, std::size_t, const std::string_view& >;
			if constexpr( std::is_same_v< bool, ReturnType > )
				return action( static_cast< const Rule& >( *rule ), pos, string );
			else
			{
				action( static_cast< const Rule& >( *rule ), pos, string );
				return true;
			}
		}

		template< typename Rules_, typename Function >
		struct ActionMediator
		{
			using Rules = Rules_;
			Function function_;

			template< typename Func_ >
			inline ActionMediator( Func_&& function ) : function_( std::forward< Func_ >( function ) ) {}

			template< typename Rule >
			inline decltype( auto ) operator()( const Rule& rule, std::size_t pos, const std::string_view& match )
			{
				return function_( rule, pos, match );
			}
		};

		struct Nothing
		{
			using Rules = std::tuple< NaR >;
		};

		template< typename Type, typename = void >
		struct action_rules
		{
			using rules = std::tuple<>;
		};

		template< typename Type >
		struct action_rules< Type, std::void_t< typename Type::Rules > >
		{
			using rules = typename Type::Rules;
		};

		template< typename Type >
		using action_rules_t = typename action_rules< Type >::rules;

		template< typename Type >
		concept IsTracingEnabled = Type::PsmTracing;

		template< typename Rule, typename ActionFunction >
		constexpr bool is_rule_action_enabled_v = !std::is_same_v< action_rules_t< ActionFunction >, std::tuple< NaR > > &&
							  ( std::tuple_size_v< action_rules_t< ActionFunction > > == 0 ||
							    detail::tuple_element_index_v< Rule, action_rules_t< ActionFunction > > != ( std::size_t )-1 );

		template< typename ActionFunction >
		using RuleActionFunction = bool ( * )( ActionFunction&, const RuleBase*, std::size_t, const std::string_view& );

		template< typename Rule, typename ActionFunction >
		constexpr RuleActionFunction< ActionFunction > getRuleActionFunction()
		{
			if constexpr( is_rule_action_enabled_v< Rule, ActionFunction > )
				return &ruleActionFunction< Rule, ActionFunction >;
			else
				return nullptr;
		}

		//===========================================================================================================================================
		// constexpr array storage

		template< typename Type, typename Values >
		struct ArrayStorage;

		template< typename Type, typename ValueType, ValueType... Values >
		struct ArrayStorage< Type, std::integer_sequence< ValueType, Values... > >
		{
			static constexpr std::array< Type, sizeof...( Values ) > array{ static_cast< Type >( Values )... };
		};

		template< typename Rules, bool Tracing >
		struct RuleConstructorArrayStorage;

		template< typename... Rules, bool Tracing >
		struct RuleConstructorArrayStorage< std::tuple< Rules... >, Tracing >
		{
			static constexpr std::array< const RuleConstructorBase*, sizeof...( Rules ) > array{ static_cast< const RuleConstructorBase* >(
			    &RuleConstructorStorage< Rules, Tracing >::constructor )... };
		};

		template< typename ActionFunction, typename Rules >
		struct RuleActionFunctionArrayStorage;

		template< typename ActionFunction, typename... Rules >
		struct RuleActionFunctionArrayStorage< ActionFunction, std::tuple< Rules... > >
		{
			static constexpr std::array< RuleActionFunction< ActionFunction >, sizeof...( Rules ) > array{ getRuleActionFunction< Rules, ActionFunction >()... };
		};
	} // namespace detail

	enum class ParsingStatus
	{
		Success,
		Fail,
		Incomplete,
		Aborted,
		Overflow
	};

	struct ParsingResult
	{
		ParsingStatus status;
		std::string_view match;
		std::strong_ordering operator<=>( const ParsingResult& other ) const noexcept = default;
	};

	class ParserBase
	{
	public:
		virtual ~ParserBase();

		inline void setStates( void* states ) noexcept { states_ = states; }

		ParsingResult parse( const char* begin, const char* end, bool complete = true );
		inline ParsingResult parse( std::string_view string, bool complete = true ) { return parse( string.data(), string.data() + string.size(), complete ); }
		template< std::size_t N >
		inline ParsingResult parse( const char ( &string )[N], bool complete = true ) { return parse( string, string + N - 1, complete ); }

		void reset();

	protected:
		struct Position;
		virtual std::size_t indexForNestedRule( std::size_t ruleIndex, std::size_t nestedIndex ) = 0;
		virtual bool callAction( RuleBase* rule, std::size_t pos, const std::string_view& string ) = 0;
		virtual void trace( const RuleInput& input, const RuleResult& result ) = 0;
		RuleBase* pushRule( std::size_t index );
		void popRule( std::size_t index );
		RuleBase* topRule( std::size_t index );
		RuleBase* previousRule( std::size_t topIntex, std::size_t prevIndex );
		inline bool isRuleStackEmpty() const noexcept;
		std::size_t inputBegin( Position* pos ) noexcept;
		std::size_t inputEnd( Position* pos, Position* matchPos, size_t extSize ) noexcept;

	protected:
		struct Position
		{
			uint16_t ruleIndex;
			uint16_t nestedIndex : 15;
			uint16_t callOnMatch : 1;
			std::size_t pos;
		};
		void* states_ = nullptr;
		detail::RuleConstructorBase const* const* pConstructors_;
		Position* pPositions_;
		Position* pPositionsEnd_;
		Position* currentPos_;
		uint8_t* ruleStackBegin_;
		uint8_t* ruleStackEnd_;
		uint8_t* stackTop_;
		std::size_t quietCounter_ = 0;
	};

	// ActionFunction is a functional object type that contains Rules (a std::tuple alias) of interesting rules
	template< typename Rule, typename ActionFunction = detail::Nothing >
	class Parser : public ParserBase
	{
		using RuleTypeInfo = detail::rule_info< Rule >;

	public:
		using UniqueRules = typename RuleTypeInfo::unique_rules;
		using Generators = typename RuleTypeInfo::generators;
		using ActionRules = typename detail::action_rules< ActionFunction >::rules;
		static constexpr std::size_t MaxDeep = RuleTypeInfo::max_deep - 1;
		static constexpr bool UseActionFunction = !std::is_same_v< ActionRules, std::tuple< NaR > >;
		static constexpr bool Tracing = detail::IsTracingEnabled< ActionFunction >;

		Parser();
		Parser( ActionFunction func );
		Parser( const Parser& ) = delete;
		Parser( Parser&& ) = delete;
		~Parser();

		inline void setActionFunction( ActionFunction func );
		inline ActionFunction& actionFunction() noexcept { return actionFunc_; }

	private:
		static constexpr std::size_t IndexArraySize = detail::dependent_rule_count< UniqueRules >::value;
		using IndexType = std::conditional_t< IndexArraySize <= 256, uint8_t, uint16_t >;
		static constexpr auto& constructors_ = detail::RuleConstructorArrayStorage< UniqueRules, Tracing >::array;
		static constexpr auto& indexes_ = detail::ArrayStorage< IndexType, detail::subrule_index_sequence_t< UniqueRules, Generators > >::array;
		static constexpr auto& offsets_ = detail::ArrayStorage< IndexType, detail::subrule_index_offset_sequence_t< UniqueRules > >::array;
		static constexpr auto& ruleActionFuncs_ = detail::RuleActionFunctionArrayStorage< ActionFunction, UniqueRules >::array;

		void initBase();

		std::size_t indexForNestedRule( std::size_t ruleIndex, std::size_t nestedIndex ) override;
		bool callAction( RuleBase* rule, std::size_t pos, const std::string_view& string ) override;
		void trace( const RuleInput& input, const RuleResult& result ) override;

	private:
		ActionFunction actionFunc_;
		std::array< Position, RuleTypeInfo::max_deep > positions_;
		std::array< uint8_t, RuleTypeInfo::stack_size > ruleStackData_;
	};

	template< typename Rule, typename ActionFunction >
	Parser< Rule, ActionFunction >::Parser()
	{
		initBase();
	}

	template< typename Rule, typename ActionFunction >
	Parser< Rule, ActionFunction >::Parser( ActionFunction func ) : actionFunc_( std::move( func ) )
	{
		initBase();
	}

	template< typename Rule, typename ActionFunction >
	Parser< Rule, ActionFunction >::~Parser()
	{
	}

	template< typename Rule, typename ActionFunction >
	inline void Parser< Rule, ActionFunction >::setActionFunction( ActionFunction func )
	{
		actionFunc_ = std::move( func );
	}

	template< typename Rule, typename ActionFunction >
	void Parser< Rule, ActionFunction >::initBase()
	{
		pConstructors_ = &constructors_.front();
		pPositions_ = &positions_.front();
		pPositionsEnd_ = &positions_.back() + 1;
		ruleStackBegin_ = &ruleStackData_.front();
		ruleStackEnd_ = &ruleStackData_.back() + 1;
		currentPos_ = &positions_.front();
		currentPos_->pos = 0;
		currentPos_->ruleIndex = ( uint16_t )( std::tuple_size_v< UniqueRules > - 1 );
		stackTop_ = &ruleStackData_.front();
	}

	template< typename Rule, typename ActionFunction >
	std::size_t Parser< Rule, ActionFunction >::indexForNestedRule( std::size_t ruleIndex, std::size_t nestedIndex )
	{
		return static_cast< std::size_t >( indexes_[offsets_[ruleIndex] + nestedIndex] );
	}

	template< typename Rule, typename ActionFunction >
	bool Parser< Rule, ActionFunction >::callAction( RuleBase* rule, std::size_t pos, const std::string_view& string )
	{
		if constexpr( !UseActionFunction )
			return true;
		if( quietCounter_ )
			return true;
		auto func = ruleActionFuncs_[currentPos_->ruleIndex];
		if( func )
			return func( actionFunc_, rule, pos, string );
		return true;
	}

	template< typename Rule, typename ActionFunction >
	void Parser< Rule, ActionFunction >::trace( const RuleInput& input, const RuleResult& result )
	{
		if constexpr( Tracing )
			actionFunc_.trace( currentPos_ - &positions_.front(), input, result,
					   static_cast< const detail::TracingRuleConstructorBase* >( pConstructors_[currentPos_->ruleIndex] )->name() );
	}

	template< typename Rule, typename ActionRules = std::tuple<>, typename ActionFunction >
	inline ParsingResult parseStringView( std::string_view string, ActionFunction&& action )
	{
		if constexpr( std::is_same_v< std::tuple<>, ActionRules > )
		{
			Parser< Rule, std::decay_t< ActionFunction > > parser( std::forward< ActionFunction >( action ) );
			return parser.parse( string );
		}
		else
		{
			Parser< Rule, detail::ActionMediator< ActionRules, std::decay_t< ActionFunction > > > parser( std::forward< ActionFunction >( action ) );
			return parser.parse( string );
		}
	}

	template< typename Rule >
	inline ParsingResult parseStringView( std::string_view string )
	{
		Parser< Rule > parser;
		return parser.parse( string );
	}
} // namespace psm