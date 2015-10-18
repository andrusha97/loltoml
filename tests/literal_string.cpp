#include "common.hpp"

#include <sstream>


namespace {
    void test_parsing(const std::string &str, const std::string &expected) {
        std::string scope = "parse literal string '" + str + "' into " + escape_string(expected);
        SCOPED_TRACE(scope);

        std::istringstream input("key = " + str);
        events_aggregator_t handler;

        loltoml::parse(input, handler);

        std::vector<sax_event_t> expected_events = {
            {sax_event_t::start_document},
            {sax_event_t::key, "key"},
            {sax_event_t::string, expected},
            {sax_event_t::finish_document}
        };

        EXPECT_EQ(expected_events, handler.events);
    }

    void test_error(const std::string &str) {
        std::string scope = "test error when parse '" + str + "'";
        SCOPED_TRACE(scope);

        std::istringstream input("key = " + str);
        events_aggregator_t handler;

        EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
    }
}


TEST(LiteralString, Simple) {
    test_parsing("''", "");
    test_parsing("'test'", "test");
}

TEST(LiteralString, NoEscape) {
    test_parsing("'  \t test \\test\"test'", "  \t test \\test\"test");
}

TEST(LiteralString, ControlCharactersAreNotAllowed) {
    test_error("'\n'");
    test_error("'\b'");
    test_error("'\x1f'");
}
