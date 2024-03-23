#include <ParsingStateMachine.h>
#include <catch2/catch_test_macros.hpp>

#if defined( _MSC_VER )
	#include <sstream>
#endif

using namespace psm;

TEST_CASE( "LogTraceAction test", "[LogTraceAction][psm]" )
{
	Parser< Plus< Ranges< 'a', 'z', '0', '9' > >, LogTraceAction > p;
#if defined( _MSC_VER )
	std::stringstream stream;
	p.actionFunction().setOutputStream( &stream );
#endif
	p.parse( "ab0" );
#if defined( _MSC_VER )
	const char text[] =
	    "  0   CallNested 0 \"\" \"Plus<Ranges<97,122,48,57>>\"\n"
	    "  1         True 0 \"a\" \"Ranges<97,122,48,57>\"\n"
	    "  0   CallNested 0 \"a\" \"Plus<Ranges<97,122,48,57>>\"\n"
	    "  1         True 0 \"b\" \"Ranges<97,122,48,57>\"\n"
	    "  0   CallNested 0 \"ab\" \"Plus<Ranges<97,122,48,57>>\"\n"
	    "  1         True 0 \"0\" \"Ranges<97,122,48,57>\"\n"
	    "  0   CallNested 0 \"ab0\" \"Plus<Ranges<97,122,48,57>>\"\n"
	    "  1   NotTrueYet 0 \"\" \"Ranges<97,122,48,57>\"\n"
	    "  0         True 0 \"ab0\" \"Plus<Ranges<97,122,48,57>>\"\n";
	CHECK( ( stream.view() == std::string_view( text, sizeof( text ) - 1 ) ) );
#endif
}