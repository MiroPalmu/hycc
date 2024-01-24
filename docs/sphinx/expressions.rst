Expression parser
=================

In expression parser whitespace token do matter.

Expression parser contains several parts which are:

- token to parentheses tree conversion
- traversing parentheses tree
- normalized expression parsing
    - decimal detection
    - literal conversion


Token to parentheses tree conversion
------------------------------------

In the list of tokens every pair of semantic scope operators :code:`(`, :code:`)`,
which do not have identifier token before :code:`(`,
and tokens inside them, are converted recursively to ad hoc parentheses token,
which contains all tokens in side them.

In the end these token lists now represent nodes in a parentheses tree,
and the token lists inside the parentheses tokens are the children of the node.

Traversing parentheses tree
---------------------------

Traversing parentheses tree is done depth-first manner,
in which at the each leaf (node without children)
normalized expression parsing is executed,
procuding new expression AST node.

Normalized expression parsing
=============================

Normalized expression parsing converts
the tokens in the list to expressions,
until none of them is left.

Literal conversion
------------------

Literals in expressions are converted to
expression nodes with function identifier :code:`__make_literal_type`,
where type is the type of literal and the only argument is
a expression node containing the string of the literal as identifier of function
and empty argument list.
Later in compilation process :code:`__make_literal_type` expressions are turned
into temporary data with type depending of the usage contexct.

Accepted conversions:

- (integer token, semantic scope oprator :code:`.`, integer token)-triplet
  -> :code:`__make_literal_fp(...)`
- integer token -> :code:`__make_literal_integer(...)`
- (literal tokens are not yet supported)

Operator parsing
----------------

Operator parsing is defined with operator patterns of tokens.

Operator parsing for operator :math::code:`@` with left associativity such that,
token list is iterated from beginning and whenever operator pattern
matches starting from some token:

- those tokens are turned into expression nodes
    - identifiers in these nodes are in form :code:`__make_operator_name`,
      where name is the name of the operator (e.g. :code:`_make_operator_unary_plus`)
    - later in the compilation process these calls are turned into
      correct function calls based on the types of arguments
- continue the search from that expression

If :math:`@` has right associativity iteration is started from the end,
such that if :math:`x` is current token in question,
then the operator patterns are tried s.t. they end at :math:`x`.

In following table :math:`@` is some operator and :math:`x` and :math:`y`
are identifiers or expressions.

.. list-table:: Operator patterns
    :widths: auto
    :header-rows: 1

    * - operator form
      - pattern
    * - :math:`@x`
      - :math:`[@, x]`
    * - :math:`x@`
      - :math:`[x, @]`
    * - :math:`x@y`
      - :math:`[x, \text{whitespace}, @, \text{whitespace}, y]`

Following operators have special operator patterns:

.. list-table:: Special operator patterns
    :widths: auto
    :header-rows: 1

    * - name
      - operator form
      - pattern
    * - call-like
      - :math:`x@...@`
      - :math:`[x, @, \cdots, @]`
    * - member access
      - :math:`x.y`
      - :math:`[x, ., y]`

Tokens forming the arguments in call-like operators are split at
semantic scope operator :code:`,` on top most level (not inside parentheses)
and these sub token lists are replaced by calling expression parsing
recursively on them. These will not be turned into :code:`__make_operator_name`
calls, but to :math:`x`.
