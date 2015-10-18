#include "common.hpp"

#include <sstream>


TEST(ArrayTable, Simple) {
    std::istringstream input("[[table]]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table_array_item, {"table"}},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(ArrayTable, NumericKey) {
    std::istringstream input("[[123]]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table_array_item, {"123"}},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(ArrayTable, QuotedKey) {
    std::istringstream input("[[\"table\\ntable\"]]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table_array_item, {"table\ntable"}},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(ArrayTable, MultipleKeys) {
    std::istringstream input("[[table.123.\"table\\ntable\"]]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table_array_item, {"table", "123", "table\ntable"}},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(ArrayTable, TableCanBeWrappedWithSpacesAndComments) {
    std::istringstream input("\r\n   \n  \t    [[table.123.\"table\\ntable\"]]   # comment\r\n\n");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table_array_item, {"table", "123", "table\ntable"}},
        {sax_event_t::comment, " comment"},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(ArrayTable, TableCanContainSpacesInside) {
    std::istringstream input("[[ table.  123.\t \"table\\ntable\" \t  ]]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::table_array_item, {"table", "123", "table\ntable"}},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(ArrayTable, TableCannotBeMultiline) {
    std::istringstream input("[[\ntable.123.\"table\\ntable\"]]");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(2, e.offset());
    }
}

TEST(ArrayTable, TableCannotBeMultiline2) {
    std::istringstream input("[[table.\n123.\"table\\ntable\"]]");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(8, e.offset());
    }
}

TEST(ArrayTable, TableCannotBeMultiline3) {
    std::istringstream input("[[table.123\n.\"table\\ntable\"]]");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(11, e.offset());
    }
}

TEST(ArrayTable, TableMustHaveAtLeastOneKey) {
    std::istringstream input("[[  ]]");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(4, e.offset());
    }
}

TEST(ArrayTable, SpacesBetweenOpeningBracketsAreNotAllowed) {
    std::istringstream input("[ [ key  ]]");
    events_aggregator_t handler;

    try {
        loltoml::parse(input, handler);
        FAIL();
    } catch (const loltoml::parser_error_t &e) {
        EXPECT_EQ(2, e.offset());
    }
}
