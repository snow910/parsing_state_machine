
# Welcome to the ParsingStateMachine

ParsingStateMachine is a C++ library for creating parsers using template-based rules. It allows you to declaratively define [Parsing Expression Grammar](https://en.wikipedia.org/wiki/Parsing_expression_grammar) (PEG) that can parse complex input strings.

Features

+ No dynamic memory allocation
    + all required memory is allocated statically inside the parser object,
    + the parser object size depends on the complexity of the rule and evaluates at compile time.
+ Allows to parse a string in parts
    + stores intermediate state to avoid re-parsing,
    + stores no pointers to a string, only its indexes.
+ Flexible rule system
    + supports common constructs like sequences, repetitions, alternations, etc,
    + rules can be nested and combined,
    + support recursive rules,
    + custom rules can be created.
+ Attach actions to rules
    + execute code when rules match,
    + access rule and match data.
+ Tracing
    + logs parsing process for debugging.

## Usage

The Parser class has two template parameters `Parser< Rule, Action >`. The second parameter specifies the type of functional object that will be called when the rule is successfully executed, can be omitted. Methods:

+ `ParsingResult parse( std::string_view string, bool complete )`,
+ `void reset()`,
+ `void setStates( void* states )`.

`ParsingResult` includes `ParsingStatus` and `std::string_view` on the matched fragment. `ParsingStatus` can take the following values:

+ `ParsingStatus::Success`,
+ `ParsingStatus::Fail`,
+ `ParsingStatus::Incomplete`,
+ `ParsingStatus::Aborted`,
+ `ParsingStatus::Overflow`.

Incomplete strings may not contain the whole sequence to satisfy the rule. In this case, the parser will return `ParsingStatus::Incomplete` and at the next call of the `parse` method will continue parsing the string from where it stopped. The string may be located at a different address, but it must contain all the characters that were in the previous call.

`States` is a pointer to some user data that will be passed to the rule object when it is executed.

By default, `Action` will be called for any successfully executed rule. If you want it to be called only for specified rules, you should define a `std::tuple` alias named Rules, where you list all such rule types. `Action` must contain an overloaded operator of the following form:

```C++
template< typename Rule >
void operator()( const Rule& rule, std::size_t pos, const std::string_view& match );
```

or if you want to be able to abort parsing:

```C++
template< typename Rule >
bool operator()( const Rule& rule, std::size_t pos, const std::string_view& match );
```

In last variant, `return false` aborts parsing.

It is also possible to suppress the call of `Action` even for the specified rules if these rules are nested in `Quite`, and the depth of nesting doesn't matter.

Alternatively, you can use the `parseStringView` function, which takes a lambda as `Action` to parsing a string.

### Recursive rules

To make a recursive rule, you must first put it into a generator (I can't find a better name for it). There are two types of generators: `GenA` (additive) and `GenM` (multiplicative). Both generators have the same template parameters `<Tag, MaxCount, Rule>`. `Tag` can be of any type and is used as a label to distinguish between different generators, `MaxCount` specifies the maximum number of recursive calls of `Rule`. It can then be referenced within `Rule` using the construct `Ref< Tag >`. Generators and `Ref` are not rules, they are used only at compile-time, and at runtime the parser goes straight to the specified rules.

Note that in general you can only reference a rule from within that rule. However, the library doesn't check this condition and in fact you can reference a rule that is in a different subbranch. This may work fine until you run out of internal memory, in which case you will get `ParsingStatus::Overflow`. If you do not violate this condition and do not exceed the `MaxCount` specified in the generator, then an overflow should not occur.

In order to understand the difference between additive and multiplicative generator let's look at the following example. Suppose we have a rule `GenM< Tag0, 3, RuleA< RuleB, GenM< Tag1, 5, RuleC > > > >`. Because of the nesting of generators into each other, it turns out that RuleA and RuleB can be in the call stack 3 times at most, and RuleC - `5 * 3 = 15` times. This multiplicative behaviour is not always necessary, sometimes you want rules to be called some specified maximum number of times, regardless of nesting. This is what `GenA` is for. Summarise the differences in a table where `+` means additivity and `*` means multiplicity.

|      | Rule | GenA | GenM |
| ---- | ---- | ---- | ---- |
| Rule |   +  |   +  |   +  |
| GenA |   *  |   +  |   *  |
| GenM |   *  |   +  |   *  |


### Example Sequence of integers

```C++
#include <ParsingStateMachine.h>
using namespace psm;

struct Action {
    using Rules = std::tuple< Plus< Range< '0', '9' > > >;
    template< typename Rule >
    void operator()( const Rule& rule, std::size_t pos, const std::string_view& match ) {
        std::cout << match << '\n';
    }
};

int main() {
    Parser< Plus< Seq< Plus< Range< '0', '9' > >, Plus< Char< ' ' > > > >, Action > parser;
    parser.parse( "101 103  107   109" );
    return 0;
}
```

## Rules

During its work, the parser creates rule objects, passes control to them and deletes rule objects. The rules themselves can be complex and contain nested rules. If necessary, rule objects can indirectly pass control to these nested rules. In this case, the object of the nested rule will be created before the object of the parent rule is destroyed. In turn, the nested rule itself can be complex and pass control to its nested rules, and those to its nested rules. This is how the stack of rule objects is formed.

Any rule must be inherited from `RuleBase` and override 1 or 2 of its virtual methods

+ `RuleResult match( RuleInputRef input, void* states )` 
+ `RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult )`

The `match` method is called immediately after the rule is created. The `input` argument contains a fragment of the input string available for matching (from `input.begin()` to `input.end()`) and a pointer to a character inside this fragment `input.current()`. The method selects the part from `input.begin()` to `input.current()` that satisfies the rule and returns RuleResult that contains the following fields:

+ `RuleMatchCode code`, can takes the following values:
    + `RuleMatchCode::True` - the selected input completely satisfies the rule,
	+ `RuleMatchCode::False` - the input not satisfies the rule,
	+ `RuleMatchCode::TrueCanMore` - the input completely satisfies the rule and can continue to satisfy it as new suitable characters become available,
	+ `RuleMatchCode::NotTrueYet` - the input does not yet satisfy the rule, but may satisfy it in the future when new suitable characters become available,
    + `RuleMatchCode::CallNested` - indirectly pass control to nested rule with input as a fragment from `input.current()` to `input.end()`,
	+ `RuleMatchCode::CallNestedOnMatch` - indirectly pass control to nested rule with input as a fragment from `input.begin()` to `input.current()`,
+ `std::size_t callIndex` - the index of nested rule.

All nested rules available for control passing from the parent rule must be listed in a `std::tuple` alias named Rules. The index of the individual rule in Rules will be the index for `RuleResult::callIndex`. After the nested rule has finished its execution, the `nestedResult` method is called. The `input` argument will contain the same data as before passing control to the nested rule, and `nestedResult` will contain the results of the nested rule execution:

+ `RuleBase* rule` - pointer to the nested rule object,
+ `std::size_t index` - its index in `Rules`,
+ `const RuleInput& input` - its input,
+ `bool result` - its result.

Note: the nested rule object is available only inside the `nestedResult` method and is destroyed immediately after exiting this method.

### `Seq` rule example

In this example Seq rule successively invokes nested rules while they are successfully executed. If one of the nested rules is not successfully executed, the main rule ends unsuccessfully.

```C++
template< typename... Rules_ >
class Seq : public RuleBase
{
public:
    using Rules = std::tuple< Rules_... >;
    RuleResult match( RuleInputRef input, void* states ) override
    {
        return { RuleMatchCode::CallNested, 0 };
    }
    RuleResult nestedResult( RuleInputRef input, void* states, const NestedRuleResult& nestedResult ) override
    {
        if( nestedResult.result )
        {
            input.setCurrent( nestedResult.input.current() );
            if( nestedResult.index == ( sizeof...( Rules_ ) - 1 ) )
                return { RuleMatchCode::True };
            return { RuleMatchCode::CallNested, nestedResult.index + 1 };
        }
        return { RuleMatchCode::False };
    }
};
```

## Build-in rules

+ [Any, AnyN< N >](#Any,-Any<-N->)
+ [Char< C... >](#Char<-C...->)
+ [NotChar< C... >](#NotChar<-C...->)
+ [Discard](#Discard)
+ [EndingWith< R, S... >](#EndingWith<-R,-S...->)
+ [End](#End)
+ [If< R, T, E >](#If<-R,-T,-E->)
+ [Not< R >](#)
+ [One< R... >](#One<-R...->)
+ [Opt< R >](#Opt<-R->)
+ [Plus< R >](#Plus<-R->)
+ [Ranges< C... >, Range< Begin, End >](#Ranges<-C...->,-Range<-Begin,-End->)
+ [NotRanges< C... >, NotRange< Begin, End >](#NotRanges<-C...->,-NotRange<-Begin,-End->)
+ [Reparse< R, S... >](#Reparse<-R,-S...->)
+ [RepMinMax< Min, Max, R >, RepMin< Min, R >, RepMax< Max, R >](#RepMinMax<-Min,-Max,-R->,-RepMin<-Min,-R->,-RepMax<-Max,-R->)
+ [Seq< R... >](#Seq<-R...->)
+ [Star< R >](#Star<-R->)
+ [Str< C... >](#Str<-C...->)
+ [Success, SuccessAll](#Success,-SuccessAll)
+ [Until< R >, UntilMax< R, Max >](#Until<-R->,-UntilMax<-R,-Max->)

#### `Any`, `Any< N >`

#### `Char< C... >`

#### `NotChar< C... >`

#### `Discard`

#### `EndingWith< R, S... >`

#### `End`

#### `If< R, T, E >`

#### `Not< R >`

#### `One< R... >`

#### `Opt< R >`

#### `Plus< R >`

#### `Ranges< C... >, Range< Begin, End >`

#### `NotRanges< C... >, NotRange< Begin, End >`

#### `[Reparse< R, S... >`

#### `RepMinMax< Min, Max, R >, RepMin< Min, R >, RepMax< Max, R >`

#### `Seq< R... >`

#### `Star< R >`

#### `Str< C... >`

#### `Success, SuccessAll`

#### `Until< R >, UntilMax< R, Max >`
