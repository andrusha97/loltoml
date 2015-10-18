#include "common.hpp"

#include <sstream>


namespace {
    void test_parsing(const std::string &str, double expected) {
        std::string scope = "parse '" + str + "' into " + std::to_string(expected);
        SCOPED_TRACE(scope);

        std::istringstream input("key = " + str);
        events_aggregator_t handler;

        loltoml::parse(input, handler);

        ASSERT_EQ(4, handler.events.size());
        EXPECT_EQ(sax_event_t(sax_event_t::start_document), handler.events[0]);
        EXPECT_EQ(sax_event_t(sax_event_t::key, "key"), handler.events[1]);
        EXPECT_EQ(sax_event_t::floating_point, handler.events[2].type);
        EXPECT_DOUBLE_EQ(expected, handler.events[2].float_data);
        EXPECT_EQ(sax_event_t(sax_event_t::finish_document), handler.events[3]);
    }

    void test_error(const std::string &str) {
        std::string scope = "test error when parse '" + str + "'";
        SCOPED_TRACE(scope);

        std::istringstream input("key = " + str);
        events_aggregator_t handler;

        EXPECT_THROW(loltoml::parse(input, handler), loltoml::parser_error_t);
    }
}


TEST(Float, Zeros) {
    test_parsing("0.0", 0.0);
    test_parsing("+0.0", 0.0);
    test_parsing("-0.0", 0.0);

    test_parsing("0e0", 0.0);
    test_parsing("-0e0", 0.0);
    test_parsing("0e-0", 0.0);
    test_parsing("0e+0", 0.0);
    test_parsing("-0e-0", 0.0);
    test_parsing("-0e+0", 0.0);
    test_parsing("+0e-0", 0.0);
    test_parsing("+0e+0", 0.0);
    test_parsing("0e10", 0.0);
    test_parsing("0e-10", 0.0);

    test_parsing("0.0e0", 0.0);
    test_parsing("0.000e+0", 0.0);
    test_parsing("0.00e-0", 0.0);
    test_parsing("-0.00e+0", 0.0);
    test_parsing("+0.00e-100", 0.0);
}

TEST(Float, Fractional) {
    test_parsing("+14.1345", 14.1345);
    test_parsing("-14.0001345", -14.0001345);
    test_parsing("0.0001345", 0.0001345);
    test_parsing("10.0", 10.0);
}

TEST(Float, Exponential) {
    test_parsing("2e3", 2000);
    test_parsing("2e-3", 0.002);
    test_parsing("2e0", 2);
    test_parsing("2e+0", 2);
    test_parsing("2e-0", 2);
    test_parsing("+2e-1", 0.2);
    test_parsing("-2e-1", -0.2);
    test_parsing("1e+123", 1e123);
    test_parsing("1e-123", 1e-123);

    test_parsing("2E3", 2000);
    test_parsing("2E-3", 0.002);
    test_parsing("2E0", 2);
    test_parsing("2E+0", 2);
    test_parsing("2E-0", 2);
    test_parsing("+2E-1", 0.2);
    test_parsing("-2E-1", -0.2);
    test_parsing("1E+123", 1e123);
    test_parsing("1E-123", 1e-123);
}

TEST(Float, FractionalExponential) {
    test_parsing("2.0e3", 2000);
    test_parsing("2.1e-3", 0.0021);
    test_parsing("2.54e0", 2.54);
    test_parsing("2.000e+0", 2);
    test_parsing("2.09e-0", 2.09);
    test_parsing("+2.2e-1", 0.22);
    test_parsing("-2.12e-1", -0.212);
    test_parsing("1.01e+123", 101e121);
    test_parsing("1.01e-123", 101e-125);

    test_parsing("2.0E3", 2000);
    test_parsing("2.1E-3", 0.0021);
    test_parsing("2.54E0", 2.54);
    test_parsing("2.000E+0", 2);
    test_parsing("2.09E-0", 2.09);
    test_parsing("+2.2E-1", 0.22);
    test_parsing("-2.12E-1", -0.212);
    test_parsing("1.01E+123", 101e121);
    test_parsing("1.01E-123", 101e-125);
}

TEST(Float, LeadingZerosAreNotAllowed) {
    test_error("01.0");
    test_error("00.0");
    test_error("02e1");
    test_error("-02e1");
    test_error("+02e-1");
    test_error("+02.01e-1");
}

TEST(Float, LeadingZerosAreNotAllowedInExponent) {
    test_error("1e01");
    test_error("1e-01");
    test_error("1.1e+01");
    test_error("-1.1e-00");
}

TEST(Float, IntegerPartCannotBeEmpty) {
    test_error(".1");
    test_error("e10");
    test_error("-.1");
    test_error(".0");
    test_error("+.0");
}

TEST(Float, FractionalPartCannotBeEmpty) {
    test_error("1.");
    test_error("1.e10");
    test_error("-1.");
    test_error("0.");
    test_error("+0.");
}

TEST(Float, ExponentCannotBeEmpty) {
    test_error("1e");
    test_error("1e+");
    test_error("1e-");
}

TEST(Float, Overflow) {
    test_error("1.0e1000");
    test_error("-1.0e1000");
}

TEST(Float, InfNanAreNotSupported) {
    test_error("Inf");
    test_error("-Inf");
    test_error("inf");
    test_error("-inf");
    test_error("INF");
    test_error("-INF");
    test_error("NAN");
    test_error("NaN");
    test_error("Nan");
    test_error("nan");
}
