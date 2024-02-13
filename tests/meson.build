# Test dependencies

subproject('ut')

test_dependencies = project_dependencies
test_dependencies += dependency('ut')

# List of tests that can run in parallel
single_threaded_unit_tests = [
    'test_unit_test',
    'test_tokenizer',
    'test_sstd',
    'test_state_pattern_matcher',
    'test_parser',
    'test_identifier_node',
    'test_function_argument_node',
    'test_type_node',
    'test_expression_node',
    'test_class_decleration_node',
    'test_function_decleration_node',
    'test_namespace_decleration_node',
    'test_data_decleration_node',
    'test_decleration_parsing_node',
    'test_scope_node',
    'test_if_statement_node',
    'test_for_loop_statement_node',
    'test_break_statement_node',
    'test_continue_statement_node',
    'test_return_statement_node',
    'test_nested_scope',
    'test_statement_scope',
    'test_namespace_scope',
    'test_function_scope',
    'test_class_scope',
    'test_ordered_property',
    'test_unordered_property',
]

single_threaded_test_names_and_exes = {}

# Create test executables:
foreach test_name : single_threaded_unit_tests
    single_threaded_test_names_and_exes += {test_name :
        executable(
            test_name,
            files(test_name + '.cpp'),
            include_directories: project_include_directories,
            dependencies: test_dependencies,
            override_options: ['optimization=0'],
        )
    }

endforeach

foreach test_name, test_exe : single_threaded_test_names_and_exes
    test(
        test_name,
        test_exe,
        is_parallel: true
    )
endforeach
