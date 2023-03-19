#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "transport_catalogue.h"
#include "log_duration.h"
#include "request_handler.h"
#include "json_reader.h"

namespace tr_cat {
namespace tests {
namespace detail {

template <typename Type1, typename Type2>
void AssertEqualImpl(const Type1& value1, const Type2 value2, 
                     const std::string& str_value1, const std::string& str_value2,
                     const std::string& file, const std::string& func, unsigned line, 
                     std::ostream& stream = std::cerr, const std::string& hint = ""s) {

    if (value1 != static_cast<Type1>(value2)) {
        stream << file << "("s << line << "): "s << func << ": ASSERT_EQUAL("s << str_value1 << ", "s << str_value2;
        stream << ") failed: "s << value1 << " != "s << value2 << "."s;
        if (!hint.empty()) {
            stream << " Hint: "s << hint;
        }
        stream << std::endl;
        abort();
    }
}

void AssertImpl(const bool value, const std::string& expr, 
                const std::string& file, const std::string& func,
                unsigned line, std::ostream& stream = std::cerr, const std::string& hint = ""s);

template <typename Func>
void RunTestImpl(Func func, const std::string& func_name, const std::string& file_in,
                            const std::string& file_out, const std::string& file_example, 
                            std::ostream& stream = std::cerr) {
    func(file_in, file_out, file_example);
    stream << func_name << " OK"s << std::endl;
}

} //detail

#define ASSERT_EQUAL(value1, value2, stream) detail::AssertEqualImpl((value1), (value2), #value1, #value2, __FILE__, __FUNCTION__, __LINE__, stream)
#define ASSERT_EQUAL_HINT(value1, value2, stream, hint) detail::AssertEqualImpl((value1), (value2), #value1, #value2, __FILE__, __FUNCTION__, __LINE__, hint, stream)

#define ASSERT(expr, stream) detail::AssertImpl((expr), #expr, __FILE__, __FUNCTION__, __LINE__, stream)
#define ASSERT_HINT(expr, stream, hint) detail::AssertImpl((expr), #expr, __FILE__, __FUNCTION__, __LINE__, hint, stream)

#define RUN_TEST(func, file_in_make, file_in_stats, file_out, file_example) detail::RunTestImpl((func), #func, (file_in), (file_out), (file_example))

void TestOutput(const std::string& file_in_make_base, const std::string& file_in_stats,
                const std::string& file_out, const std::string& file_example, std::ostream& stream);
size_t TestCatalogSpeed(const std::string& file_in_make_base, const std::string& file_out_stats,
                      const std::string& file_out, const std::string& render_file, std::ostream& stream);
void Test(const std::string file_in_make_base, const std::string file_in_stats,
          const std::string file_out, const std::string file_example, 
          const std::string file_map, std::ostream& stream);
}//tests
}//tr_cat
