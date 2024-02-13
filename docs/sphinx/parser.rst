Parsing
=======

Parsing is defined in terms of human readable pseudo code
which executes different actions.
Implementations has to only appear that they do the same thing.

Action pointers
---------------

Action pointer is defined to be abstract pointer pointing at action
in some set of orderd actions.
Whenever action pointer is executed the action pointer is
incremented to the next action (*).

Action pointer stack
--------------------

During parsing stack of action pointers is kept.
One can :code:`push` action pointer to the top or :code:`pop` action pointer from top.
Parses executes top of the stack until the stack is empty.
Pushing a node means that pointer to the first action of node is :code:`pushed`.

(*) If there is no next action the action pointer is :code:`popped`.

Pattern matching
----------------

Place of next unparsed token is kept. Patterns of token types
can be tried to match next tokens, and if succesful those
tokens are marked as parsed and action pointer pointing to first
action of the pattern is :code:`pushed`.

Actions in nodes refer to this matching as just trying to match pattern.

Initialization
--------------

Initially stack contains action pointer to the first action of:

- create a (global scope) scope node
- add intrinsics to the global scope
- :code:`push` global scope node

AST
---

Below is a visualization of all possible nodes in AST and
how they can relate to each other. Arrows tell direction
of descension.

.. graphviz:: node_relations.dot


Leafs are normal nodes, but with guarantee that
when descending down the AST from leafs
only recurses to anohter leafs.

Abstract nodes/leafs are not actual nodes in AST,
but to give vocabulary for different relations.

Backtracking
------------

Each node stores a pointer to the parent node,
so the tree can be ascendend.

Contracts
---------

Nodes can specify pre and post contracts, which are assumend to be
fulfilled before of the first or the last action of the node respectivly.
Failure to fulfill contracts is internal compiler error (ICE).

Patterns
--------

Nodes define different patterns of tokens.
Thsee patterns assume that whitespace tokens are ignored.

When node tries to match patterns,
each pattern is tried one-by-one in order and if some pattern matches
pointer to the first action of the patter is :code:`pushed`.

Syntax error is defined as situation when none of the patterns match.
So empty pattern can be used to indicate that it is ok,
if no previous patterens matched.

Patterns are defined as series of different token types.
Token type :code:`X` can be specialized for speicific token using regex :code:`reg`
with :code:`X: reg`. Token type can be optional if denoted with :code:`[token_type]`.
More abstract patterns are indicated with
some description enclosed with angle brackets :code:`<description>`.

Nodes
-----

Identifier node
^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - next unparsed tokens are in form:
        - mix of identifier tokens and semantic scope operator :code:`:` pairs
        - until not

Properties
""""""""""

List of ordered identifier tokens and scope resolution operators.

Actions
"""""""
- match patterns

Patterns
""""""""
Scope resolution operator
    - pattern:
        - sematic scope operator: :code:`:`
        - sematic scope operator: :code:`:`
    - actions:
        - add scope resolution operator to list of identifiers

Identifier
    - pattern:
        - identifier token
    - actions:
        - add matched identifier token to list of identifier tokens

End of identifier
    - pattern:
    - actions:

Function argument node
^^^^^^^^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed token is :code:`(`
    - next unparsed tokens are in form:
        - [identifie token: some passing type]
        - identifier token
        - semantic scope operator :code:`:`
        - tokens to construct type
        - sematic scope operator :code:`,` or :code:`)`
        - repeate until it was :code:`)`
Post
    - latest parsed token is :code:`)`

Properties
""""""""""

Arguments:
    - passing type
    - identifier
    - type

Actions
"""""""

- match patterns

Patterns
""""""""

Passing type
    - pattern:
        - identifier tokens: :code:`in`, :code:`inout`, :code:`out`, :code:`move`, :code:`copy` or :code:`forward`
    - action:
        - add new argument to the list
        - set its passing type to what was mathced

Argument identifier
    - pattern:
        - identifier token
    - action:
        - if latests argument in the list does not have name:
            - set its identifier to the matched identifier token
            - :code:`pop`
        - add new argument to the list
        - set its passing type to :code:`out`
        - set its identifier to the matched identifier token

Type spearator
    - pattern:
        - sematic scope operator: :code:`:`
    - action:
        - create a new type node to the latest argument
        - :code:`push` that node

Argument separator
    - pattern:
        - sematic scope operator: :code:`,`
    - action:

End of arguments
    - pattern:
        - semantic scope operator: :code:`)`
    - action:

Type node
^^^^^^^^^

Contract
""""""""

Pre
    - next unparsed tokens are in form:
        - [identifier token: :code:`const`]
        - [operator: :code:`*`]
        - tokens that form identifier

Properties
""""""""""

- is function, pointer or regular type
- is const
- if function type
    - argument types
    - return type
- if pointer type
    - pointed type
- if regular type
    - identifier

Actions
"""""""

- match patterns

Patterns
""""""""

Constness
    - pattern:
        - identifier token: :code:`const`
    - action:
        - set this type to be const

Function arguments
    - pattern:
        - semantic scope operator: :code:`(`
    - action:
        - mark this type to be function
        - create argument type node
        - :code:`push` that node

Function return type separator
    - pattern:
        - operator token: :code:`->`
    - action:
        - create return type node
        - :code:`push` that node

Pointer
    - pattern:
        - operator token: :code:`*`
    - action:
        - mark this type to be pointer
        - create pointed type node
        - :code:`push` that node

Regular type
    - pattern:
    - action:
        - mark this type to be a regular type
        - create identifier node
        - :code:`push` that node

End of type
    - pattern:
    - action:

Expression node
^^^^^^^^^^^^^^^

Expression node represent one function call.

Contracts
"""""""""

Pre
    - next unparsed tokens form expressions
    - after which there is scope operator token :code:`;`

Post
    - latest parsed token is scopen operator token :code:`;`

Properties
""""""""""

- identifier of function
- list of arguments represented as other expressions

Actions
"""""""

- match until semantic scope operator :code:`;` is found
- send matched tokens (except semantic scope operator :code:`;`) to expression parser
- set this node to the one that was returned from expression parser

Class decleration node
^^^^^^^^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed tokens are:
        - <forms identifier node>
        - semantic scope operator: :code:`:`
        - identifier token: :code:`type`
        - operator: `=`
    - this nodes identifier is set

Properties
""""""""""

- identifier
- class scope

Actions
"""""""

- match one pattern

Patterns
""""""""

Class scope
    - pattern:
        - semantic scope operator: :code:`{`
    - actions:
        - create class scope node
        - :code:`push` that node

Function decleration node
^^^^^^^^^^^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed tokens are:
        - <forms identifier node>
        - semantic scope operator: :code:`:`
        - <forms function type>
        - operator: `=`
    - this nodes identifier is set
    - this nodes type is set to some function type

Properties
""""""""""

- identifier
- type
- function scope

Actions
"""""""

- match one patterns

Patterns
""""""""

Function scope
    - pattern:
        - semantic scope operator: :code:`{`
    - actions:
        - create funciton scope
        - :code:`push` that node

Namespace decleration node
^^^^^^^^^^^^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed tokens are:
        - <forms identifier node>
        - semantic scope operator: :code:`:`
        - identifier token: :code:`namespace`
        - operator: `=`
    - this nodes identifier is set
    - next unparsed tokens form scope

Properties
""""""""""

- identifier
- namespace scope

Actions
"""""""

- match one patterns

Patterns
""""""""

Namespace scope
    - pattern:
        - semantic scope operator: :code:`{`
    - actions:
        - create namespace scope
        - :code:`push` that node

Data decleration node
^^^^^^^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed tokens are:
        - <forms identifier node>
        - semantic scope operator: :code:`:`
        - <forms type node>
    - this nodes identifier is set
    - next unparsed token is semantic scope operator :code:`;`,
      or operator:`=`

Properties
""""""""""

- identifier
- type
- optional expression

Actions
"""""""

- :code:`push` that node
- match one patterns

patterns
""""""""

Definition
    pattern:
        - operator: :code:`=`
    actions:
        - create expression
        - :code:`push` that node

No-definition
    pattern:
        - semantic scope operator: :code:`;`
    actions:
        - noop

Decleration parsing node
^^^^^^^^^^^^^^^^^^^^^^^^

This node is helper node to parse declerations.
It is not valid part of AST.

Contracts
"""""""""

Pre
    - latest parsed tokens are:
        - <forms identifier node>
        - semantic scope operator: :code:`:`
    - next unparsed tokens either:
        - is identifier: :code:`type`
        - is identifier: :code:`namespace`
        - forms type
    - this nodes identifier is set

Post
    - decleration kind matches the decleration node

Properties
""""""""""

- identifier
- decleration kind:
    - class
    - namespace
    - data
    - function
- decleration node

Actions
"""""""

- match one patterns

Patterns
""""""""

Class delceration
    - pattern:
        - identifier: :code:`type`
        - operator: :code:`=`
    - actions:
        - set decleration kind to class
        - create class decleration node
        - set its identifier from this nodes identifier
        - :code:`push` that node

Namespace delceration
    - pattern:
        - identifier: :code:`namespace`
        - operator: :code:`=`
    - actions:
        - set decleration kind to namespace
        - create namespace decleration node
        - set its identifier from this nodes identifier
        - :code:`push` that node

Data or function delceration
    - pattern:
    - actions:
        - create temporary type node T
        - :code:`push` that node
        - if node T is function type:
            - set decleration kind to function
            - match operator :code:`=`
            - create function decleration node
            - :code:`push` that node
        - if node T is not function type:
            - set decleration kind to data
            - create data decleration node
            - :code:`push` that node

Scope node
^^^^^^^^^^

Contracts
"""""""""

Pre
    - previous parsed token is semantic scope operator: :code:`{`

Post
    - previous parsed tokens are:
        - sematic scope operator: :code:`}`, :code:`;` or both

Properties
""""""""""

- list of ordered nodes
    - nested scope
    - statement
    - namespace decleration
    - data decleration
    - expression
- set of unordered nodes
    - function decleration
    - class decleration

Actions
"""""""

- match patterns until all tokens are parsed

Patterns
""""""""

Nested scope
    - pattern:
        - semantic scope operator: :code:`{`
    - actions:
        - create nested scope node
        - :code:`push` that nodes scope node

Statement
    - pattern:
        - identifier token: :code:`if`, :code:`for`, :code:`break`, :code:`continue` or :code:`return`
    - actions:
        - create corresponding statement node
        - :code:`push` that node

Decleration
    - pattern:
        - <forms identifier>
        - semantic scope operator: :code:`:`
    - actions:
        - create temporary decleration parsing node T
        - set its identifier
        - :code:`push` that node
        - move the decleration node inside node T to this scope

End of scope
    - pattern:
        - semantic scope operator: :code:`}`
    - actions:

Expression
    - pattern:
    - actions:
        - create expression node
        - push that node



If statement
^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed token is identifier token: :code:`if`
    - next unparsed tokens form expression

Properties
""""""""""

- tested expression
- statement scope
- else-if-statement

Actions
"""""""

- create tested expression node
- :code:`push` that node
- match one pattern
- match next tokens to identifier token :code:`else` followed by identifier token :code:`if`:
    - create else-if-statement node
    - :code:`push` that node
- match next tokens to identifier token :code:`else`:
    - create else-if-statement node
    - set thats node tested expression to true
    - match one pattern inside that node

Patterns
""""""""

Scoped
    - pattern:
        - semantic scope operator: :code:`{`
    - actions:
        - create statement scope node
        - :code:`push` that node

.. Out of MVP.
.. - single expression:
..     - pattern:
..     - actions:
..         - create statement scope node
..         - create single ordered expression node inside it
..         - :code:`push` that node

For loop statement
^^^^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed token is identifier token: :code:`for`
    - next unparsed tokens form scope where which contains 3 expressions

Properties
""""""""""

- for-condition scope
- for-statement scope

Actions
"""""""

- match semantic scope operator :code:`{`
- create for-condition scope node
- :code:`push` that node
- match one patterns

Patterns
""""""""

Scoped
    - pattern:
        - semantic scope operator: :code:`{`
    - actions:
        - create for-statement scope node
        - :code:`push` that node

.. Out of MPV:
.. - single expression:
..     - pattern:
..     - actions:
..         - create (for statement) scope node
..         - create single ordered expression node inside it
..         - :code:`push` that node

Continue statement
^^^^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed token is identifier token: :code:`continue`
    - next unparsed token is semantic scope operator :code:`;`

Actions
"""""""

- match one pattern

Patterns
""""""""

- end
    - patterns:
        - semantic scope operator: :code:`;`
    - actions:


Break statement
^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed token is identifier token: :code:`break`
    - next unparsed token is semantic scope operator :code:`;`

Actions
"""""""

- match one pattern

Patterns
""""""""

- end
    - patterns:
        - semantic scope operator: :code:`;`
    - actions:

Continue statement
^^^^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed token is identifier token: :code:`continue`

Return statement
^^^^^^^^^^^^^^^^

Contracts
"""""""""

Pre
    - latest parsed token is identifier token: :code:`return`
    - next unparsed tokens form expression

Properties
""""""""""

- returned expression

Actions
"""""""

- create returned expression node
- :code:`push` that node


Nodes planned but out of MVP
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- alias
