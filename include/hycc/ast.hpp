#pragma once

#include <utility>
#include <variant>
#include <vector>

#include "hycc/tokenizer.hpp"

namespace hycc {
namespace ast {

class identifier_node {};
class function_argument_node {};
class type_node {};
class expression_node {};
class class_decleration_node {};
class function_decleration_node {};
class namespace_decleration_node {};
class data_decleration_node {};
class decleration_parsing_node {};
class scope_node {};
class if_statement_node {};
class for_loop_statement_node_ {};
class break_statement_node {};
class continue_statement_node {};
class return_statement_node {};

class nested_scope : public scope_node {};
class statement_scope : public scope_node {};
class namespace_scope : public scope_node {};
class function_scope : public scope_node {};
class class_scope : public scope_node {};

using ordered_property   = std::variant<nested_scope,
                                      if_statement_node,
                                      for_loop_statement_node_,
                                      break_statement_node,
                                      continue_statement_node,
                                      return_statement_node,
                                      namespace_decleration_node,
                                      expression_node,
                                      data_decleration_node>;

using unordered_property = std::variant<function_decleration_node, class_decleration_node>;

} // namespace ast
} // namespace hycc
