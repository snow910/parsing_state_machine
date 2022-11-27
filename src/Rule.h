#pragma once

#include <stdint.h>
#include <string_view>
#include <tuple>

namespace psm
{
	class RuleInput
	{
		const char *current_, *begin_, *end_;

	public:
		inline RuleInput( const char* begin, const char* end ) noexcept
		    : current_( begin ), begin_( begin ), end_( end ) {}
		inline RuleInput( const char* begin, const char* end, const char* current ) noexcept
		    : current_( current ), begin_( begin ), end_( end ) {}

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

	template< std::size_t Index >
	struct Tag
	{
	};

	using Tag0 = Tag< 0 >;
	using Tag1 = Tag< 1 >;
	using Tag2 = Tag< 2 >;
	using Tag3 = Tag< 3 >;
	using Tag4 = Tag< 4 >;
	using Tag5 = Tag< 5 >;
	using Tag6 = Tag< 6 >;
	using Tag7 = Tag< 7 >;
	using Tag8 = Tag< 8 >;
	using Tag9 = Tag< 9 >;

	template< typename Tag_, std::size_t MaxCount_, typename Rule_ >
	struct Gen
	{
		static_assert( MaxCount_ > 0, "" );
		using Tag = Tag_;
		using Rule = Rule_;
		static constexpr std::size_t MaxCount = MaxCount_;
	};

	template< typename Tag_ >
	struct Ref
	{
		using Tag = Tag_;
	};

	struct NaR // not a rule
	{
	};

	template< typename Rule >
	class Quiet : public Rule
	{
	};
} // namespace psm