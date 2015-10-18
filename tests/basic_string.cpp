#include "common.hpp"

#include <sstream>


namespace {
    void test_parsing(const std::string &str, const std::string &expected) {
        std::string scope = "parse basic string '" + str + "' into " + escape_string(expected);
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


TEST(BasicString, Simple) {
    test_parsing("\"\"", "");
    test_parsing("\"test\"", "test");
}

TEST(BasicString, Escape) {
    test_parsing("\"test\\b\\t\\n\\f\\r\\\"\\\\\"", "test\b\t\n\f\r\"\\");
    test_parsing("\"\\u0001\"", "\x01");
    test_parsing("\"\\U00000001\"", "\x01");
    test_parsing("\"\\U000000e9\"", "é");
    test_parsing("\"test\\ud7fftest\"", "test\xed\x9f\xbftest");
    test_parsing("\"test\\ud7FFtest\"", "test\xed\x9f\xbftest");
    test_parsing("\"test\\ue000test\"", "test\xee\x80\x80test");
    test_parsing("\"test\\uE000test\"", "test\xee\x80\x80test");
    test_parsing("\"test\\U0000e000test\"", "test\xee\x80\x80test");
    test_parsing("\"test\\U0000E000test\"", "test\xee\x80\x80test");
    test_parsing("\"test\\U0010fffftest\"", "test\xf4\x8f\xbf\xbftest");
    test_parsing("\"test\\U0010FFFFtest\"", "test\xf4\x8f\xbf\xbftest");
}

TEST(BasicString, ControlCharactersMustBeEscaped) {
    test_error("\"test\ntest\"");
    test_error("\"test\0test\"");
    test_error("\"test\btest\"");
}

TEST(BasicString, BadEscape) {
    test_error("\"test\\mtest\"");
    test_error("\"test\\xtest\"");
    test_error("\"test\\ltest\"");
}

TEST(BasicString, BadUnicode) {
    test_error("\"test\\ud800test\"");
    test_error("\"test\\ud900test\"");
    test_error("\"test\\udffftest\"");
    test_error("\"test\\U0000d800test\"");
    test_error("\"test\\U0000d900test\"");
    test_error("\"test\\U0000dffftest\"");

    test_error("\"test\\uD800test\"");
    test_error("\"test\\uD900test\"");
    test_error("\"test\\uDfFFtest\"");
    test_error("\"test\\U0000D800test\"");
    test_error("\"test\\U0000D900test\"");
    test_error("\"test\\U0000DFFFtest\"");

    test_error("\"test\\U00110000test\"");
    test_error("\"test\\Uaa110000test\"");
}
