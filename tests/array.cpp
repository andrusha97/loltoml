#include "common.hpp"

#include <sstream>


TEST(Array, Empty) {
    std::istringstream input("key = []");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::finish_array, 0},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, EmptyWithSpaces) {
    std::istringstream input("key = [  \t\n \r\n  \t\t  ]\r\n ");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::finish_array, 0},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, OneLineWithItem) {
    std::istringstream input("key = [6]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 6},
        {sax_event_t::finish_array, 1},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, OneLineWithItemAndSpaces) {
    std::istringstream input("key = [ \t6  ]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 6},
        {sax_event_t::finish_array, 1},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, MultiLineWithItem) {
    std::istringstream input("key = [\n\r\n \t6 \r\n \n]\r\n ");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 6},
        {sax_event_t::finish_array, 1},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, OneLineWithMultipleItems) {
    std::istringstream input("key = [1,2,3]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::integer, 2},
        {sax_event_t::integer, 3},
        {sax_event_t::finish_array, 3},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, OneLineWithMultipleItemsAndSpaces) {
    std::istringstream input("key = [  1\t , \t  2 , 3  \t]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::integer, 2},
        {sax_event_t::integer, 3},
        {sax_event_t::finish_array, 3},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, MultiLineWithMultipleItems) {
    std::istringstream input("key = [ \r\n 1\t \n,#comment\n \t  2 ,\r\n 3  \t # one more comment\n]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::comment, "comment"},
        {sax_event_t::integer, 2},
        {sax_event_t::integer, 3},
        {sax_event_t::comment, " one more comment"},
        {sax_event_t::finish_array, 3},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, MultiLineWithMultipleTypesOfStrings) {
    std::istringstream input("key = ['literal', '''multiline\nliteral''', \"basic\", \"\"\"\n  multiline\nbasic\"\"\"]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::string, "literal"},
        {sax_event_t::string, "multiline\nliteral"},
        {sax_event_t::string, "basic"},
        {sax_event_t::string, "  multiline\nbasic"},
        {sax_event_t::finish_array, 4},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, ArrayOfArrays) {
    std::istringstream input("key = [[1,2,3], [], ['a', 'b']]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::integer, 2},
        {sax_event_t::integer, 3},
        {sax_event_t::finish_array, 3},
        {sax_event_t::start_array},
        {sax_event_t::finish_array, 0},
        {sax_event_t::start_array},
        {sax_event_t::string, "a"},
        {sax_event_t::string, "b"},
        {sax_event_t::finish_array, 2},
        {sax_event_t::finish_array, 3},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, ArrayOfTables) {
    std::istringstream input("key = [{}, { a = 1, \"b\" = 2, 2 = 3}]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::start_inline_table},
        {sax_event_t::finish_inline_table, 0},
        {sax_event_t::start_inline_table},
        {sax_event_t::key, "a"},
        {sax_event_t::integer, 1},
        {sax_event_t::key, "b"},
        {sax_event_t::integer, 2},
        {sax_event_t::key, "2"},
        {sax_event_t::integer, 3},
        {sax_event_t::finish_inline_table, 3},
        {sax_event_t::finish_array, 2},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, TrailingComma) {
    std::istringstream input("key = [1,2,3,]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::integer, 2},
        {sax_event_t::integer, 3},
        {sax_event_t::finish_array, 3},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, TrailingCommaWithOneItem) {
    std::istringstream input("key = [1,]");
    events_aggregator_t handler;

    loltoml::parse(input, handler);

    std::vector<sax_event_t> expected_events = {
        {sax_event_t::start_document},
        {sax_event_t::key, "key"},
        {sax_event_t::start_array},
        {sax_event_t::integer, 1},
        {sax_event_t::finish_array, 1},
        {sax_event_t::finish_document}
    };

    EXPECT_EQ(expected_events, handler.events);
}

TEST(Array, AllItemsMustBeOfOneType) {
    std::istringstream input("key = [1, 2.0]");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}

TEST(Array, AllItemsMustBeOfOneType2) {
    std::istringstream input("key = [1, 'abc']");
    events_aggregator_t handler;

    EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
}
