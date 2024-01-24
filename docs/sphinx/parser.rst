Parsing
=======

Action pointers
---------------

Parsing is defined as series of abstract actions.
Action pointer is defined to be abstract pointer pointing at action,
which can be executed.
whenever action pointer is executed the action pointer is
incremented to the next action (*).

Action pointer stack
--------------------

During execution stack of action pointers is kept.
One can :code:`push` action pointer to the top or :code:`pop` action pointer from top.
Parses executes top of the stack until the stack is empty.
Pushing a node means that pointer to the first action of node is pushed.

(*) If there is no next action the action pointer is :code:`popped`.

Pattern matching
----------------

Place of next unparsed token is kept. Patterns of token types
can be tried to match next tokens and if succesful then those
tokens are marked as parsed and action pointer pointing to first
action of the pattern is :code:`pushed`. Actions refer to this matching
as just trying to match pattern.

Initialization
--------------

Initially stack contains action pointer to the first action of:

- create a (global scope) scope node
- add intrinsics to the global scope
- :code:`push` global scope node
- :code:`pop`

AST
===

Nodes
-----

Below is a visualization of all possible nodes in AST and
how they can relate to each other. Arrows tell direction
of descension.

.. graphviz:: node_relations.dot


Leafs are normal nodes, but with guarantee that
when descending down the AST from leafs,
it is impossible to find scope node.

Abstract nodes/leafs are not actual nodes in AST,
but to give vocabulary for different relations.

Backtracking
------------

Each node stores a pointer to the parent node,
so the tree can be backtracked.

Contracts
---------

Nodes can specify pre and post contracts, which are assumend to be
fulfilled. Failure to fulfill contracts is internal compiler error.
Pre contract should be fulfilled just before :code:`pushing` some node and
post contract after :code:`popping` a node.

Patterns
--------

Nodes define different patterns. When node tries to match patterns,
each pattern is tried one-by-one in order and if some pattern matches
pointer to the first action of the patter is :code:`pushed`.

Syntax error is defined as situation when none of the patterns match.

Patterns are defined as series of different token types.
Token type :code:`X` can be specialized for speicific token using regex :code:`reg`
with :code:`X: reg`. Token type can be optional if denoted with :code:`[token_type]`.

Empty pattern in is used to indicate that it is ok,
if no previous patterens matched.

Identifier node
---------------

Contracts:

- pre:
    - next unparsed tokens are in form:
        - mix of identifier tokens and semantic scope operator :code:`:` pairs
        - until not

Properties:

- list of ordered identifier tokens and scope resolution operators

Actions:

- match patterns until end of identifier is reached

Patterns:

- scope resolution operator:
    - pattern:
        - sematic scope operator: :code:`:`
        - sematic scope operator: :code:`:`
    - actions:
        - add scope resolution operator to list of identifiers
- identifier:
    - pattern:
        - identifier token
    - actions:
        - add matched identifier token to list of identifier tokens
- end of identifier:
    - pattern:
    - actions:
        - noop

Function argument node
----------------------

Contracts:

- pre:
    - latest parsed token is :code:`(`
    - next unparsed tokens are in form:
        - [identifie token: some passing type]
        - identifier token
        - semantic scope operator :code:`:`
        - tokens to construct type
        - sematic scope operator :code:`,` or :code:`)`
        - repeate until it was :code:`)`
- post:
    - latest parsed token is :code:`)`

Properties:

- list of arguments:
    - passing type
    - identifier
    - type

Actions:

- match patterns until end of arguments is reached

Patterns:

- passing type:
    - pattern:
        - identifier tokens: :code:`in`, :code:`inout`, :code:`out`, :code:`move`, :code:`copy` or :code:`forward`
    - action:
        - add new argument to the list
        - set its passing type to what was mathced
- argument identifier:
    - pattern:
        - identifier token
    - action:
        - if latests argument in the list does not have name:
            - set its identifier to the matched identifier token
            - :code:`pop`
        - add new argument to the list
        - set its passing type to :code:`out`
        - set its identifier to the matched identifier token
- type spearator:
    - pattern:
        - sematic scope operator: :code:`:`
    - action:
        - create a new type node to the latest argument
        - :code:`push` that node
- argument separator:
    - pattern:
        - sematic scope operator: :code:`,`
    - action:
- end of arguments:
    - pattern:
        - semantic scope operator: :code:`)`
    - action:
        - noop

Type node
---------

Contracts

- pre:
    - next unparsed tokens are in form:
        - [identifier token: :code:`const`]
        - [operator: :code:`*`]
        - tokens that form identifier

Properties:

- is function, pointer or regular type
- is this type const
- if function type:
    - argument types
    - return type
- if pointer type:
    - pointed type
- if regular type:
    - identifier

Actions:

- match patterns until end of type is reached

Patterns:

- constness:
    - pattern:
        - identifier token: :code:`const`
    - action:
        - set this type to be const
- function arguments:
    - pattern:
        - semantic scope operator: :code:`(`
    - action:
        - mark this type to be function
        - create argument type node
        - :code:`push` that node
- function return type separator:
    - pattern:
        - operator token: :code:`->`
    - action:
        - create return type node
        - :code:`push` that node
- pointer:
    - pattern:
        - operator token: :code:`*`
    - action:
        - mark this type to be pointer
        - create pointed type node
        - :code:`push` that node
- regular type:
    - pattern:
    - action:
        - mark this type to be a regular type
        - create identifier node
        - :code:`push` that node
- end of type:
    - pattern:
    - action:

Expression node
---------------

Expression node represent one function call.

Contracts:

- pre:
    - next unparsed tokens form expressions
    - after which there is scope operator token :code:`;`
- post:
    - latest parsed token is scopen operator token :code:`;`

Properties:

- identifier of function
- list of arguments represented as other expressions

Actions:

- match until semantic scope operator :code:`;` is found
- send matched tokens (except semantic scope operator :code:`;`) to expression parser
- set this node to the one that was returned from expression parser

Alias node
----------

Contracts:

- pre:
    - latest parsed token is alias :code:`using`

TODO (waiting for alias documentation)

Class decleration node
----------------------

Contracts:

- pre:
    - latest parsed tokens are:
        - form identifier
        - semantic scope operator: :code:`:`
        - identifier token: :code:`type`

TODO

Scope node
----------

Contracts:

- pre:
    - previous parsed token is semantic scope operator: :code:`{`
- post:
    - previous parsed tokens are:
        - sematic scope operator: :code:`}`, :code:`;` or both

Properties:

- list of ordered nodes
    - nested scope
    - statement
a   - namespace decleration
    - alias
u   - data decleration
?   - expression
- set of unordered nodes
a   - function decleration
u   - class decleration

Actions:

- mathch one pattern

Patterns:

- nested scope:
    - pattern:
        - semantic scope operator: :code:`{`
    - actions:
        - create nested scope node
        - :code:`push` that nodes scope node
- statement:
    - pattern:
        - identifier token: :code:`if`, :code:`for`, :code:`break`, :code:`continue` or :code:`return`
    - actions:
        - create corresponding statement node
        - :code:`push` that node
- alias:
    - pattern:
        - identifier token: :code:`using`
    - actions:
        - create alias node
        - :code:`push` that node
- class decleration:
    - pattern:
        - identifier token
        - semantic scope operator: :code:`:`
        - identifier token: :code:`type`
    -
- data decleration:

- namespace decleration:


- class declaration:
    - pattern:
        - identifier token
        - semantic scope operator: :code:`:`
        - identifier token: :code:`type`
        - sematic: `
    - actions:
        - add node to list of class definition nodes
        - set name of that node
        - goto that node
- function declaration:
    - pattern:
        - identifier token
        - semantic scope operator: :code:`:`
        - semantic scope operator: :code:`(`
    - actions:
        - add node to list of function declaration nodes
        - set name of that node
        - add argument scope node to that node
        - goto that argument scope node

If statement
------------

Contracts:

- pre:
    - latest parsed token is identifier token: :code:`if`
    - next unparsed tokens form expression

Properties:

- tested expression
- statement scope
- else-if-statement

Actions:

- create tested expression node
- :code:`push` that node
- match one pattern
- match next tokens to identifier token :code:`else` followed by identifier token :code:`for`:
    - create else-if-statement node
    - :code:`push` that node
- match next tokens to identifier token :code:`else`:
    - create else-if-statement node
    - set thats node tested expression to true
    - match one pattern inside that node

Patterns:

- scoped:
    - pattern:
        - semantic scope operator: :code:`{`
    - actions:
        - create (statement) scope node
        - :code:`push` that node
- single expression:
    - pattern:
    - actions:
        - create (statement) scope node
        - create single ordered expression node inside it
        - :code:`push` that node

For loop statement
------------------

Contracts:

- pre:
    - latest parsed token is identifier token: :code:`for`
    - next unparsed tokens form scope where which contains 3 expressions

Properties:

- for condition scope
- for statement scope

Actions:

- match semantic scope operator :code:`{`
- create (for condition) scope node
- push that node
- match patterns

Patterns:

- scoped:
    - pattern:
        - semantic scope operator: :code:`{`
    - actions:
        - create (for statement) scope node
        - :code:`push` that node
- single expression:
    - pattern:
    - actions:
        - create (for statement) scope node
        - create single ordered expression node inside it
        - :code:`push` that node

Continue statement
------------------

Contracts:

- pre:
    - latest parsed token is identifier token: :code:`continue`

Break statement
---------------

Contracts:

- pre:
    - latest parsed token is identifier token: :code:`break`

Continue statement
------------------

Contracts:

- pre:
    - latest parsed token is identifier token: :code:`continue`

Return statement
----------------

Contracts:

- pre:
    - latest parsed token is identifier token: :code:`return`
    - next unparsed tokens form expression

Properties:

- returned expression

Actions:

- create returned expression node
- push that node


...................................................................


Scope node
----------


Properties:

- list of ordered nodes
    - statement
    - namespace decleration
    - alias
    - data decleration
    - expression
- set of unordered nodes
    - function decleration
    - class decleration

Patterns:

- Statement:
    - pattern:
        - identifier token: :code:`if`


- Class declaration:
    - pattern:
        - identifier token
        - semantic scope operator: :code:`:`
        - identifier token: :code:`type`
        - sematic: `
    - actions:
        - add node to list of class definition nodes
        - set name of that node
        - goto that node
- Function declaration:
    - pattern:
        - identifier token
        - semantic scope operator: :code:`:`
        - semantic scope operator: :code:`(`
    - actions:
        - add node to list of function declaration nodes
        - set name of that node
        - add argument scope node to that node
        - goto that argument scope node

Actions:

- match patterns
- pop

Class definition node
---------------------

Properties:

- name
- class scope node

Patterns:

- Class definition:
    - pattern:
        - operator: :code:`=`
        - semantic scope operator: :code:`{`
    - actions:
        - create empty class scope node
        - goto that node
