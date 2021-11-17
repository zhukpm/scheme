add_catch(test_scheme
        test_boolean.cpp
        test_control_flow.cpp
        test_eval.cpp
        test_integer.cpp
        test_lambda.cpp
        test_list.cpp
        test_symbol.cpp
        scheme_test.cpp)

target_link_libraries(test_scheme libscheme)
