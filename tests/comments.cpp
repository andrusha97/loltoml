#include "common.hpp"

#include <sstream>


TEST(Comments, EmptyDocumentWithComments) {
    std::istringstream input("#first comment\n\n   \t# second\r\n\r\n #last one without new-line after it");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::comment, "first comment"},
        {sax_event_t::comment, " second"},
        {sax_event_t::comment, "last one without new-line after it"},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Comments, CommentCanContainHorizontalTab) {
    std::istringstream input("# this comment has a tab \t <-- here it is");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::comment, " this comment has a tab \t <-- here it is"},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Comments, CommentsCannotContainControlCharacters) {
    std::istringstream input("# this comment ends here -->\b and after that will be an error");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(28, e.offset());
    }
}
