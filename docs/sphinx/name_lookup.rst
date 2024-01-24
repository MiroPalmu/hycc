Name lookup
===========

Name lookup can not be done during parsing due to the node missing,
which is refered by the identifier.
So name lookup has to be done after parsing.

Because functions can be overloaded function name lookup results in
list of possible functions. Which function is then chosen from these is
job of oveload resolution. For everything else shadowing concept is applied
and the first match upwards in ast is chosen.

Descendable declerations
------------------------

Descendable declerations are declerations that name lookup can descend.
All descendable declerations have child/inner scope which can be queried
for decleration :math:`d` as :math:`C(d)`. Descendable declerations are:

- namespace
- class

Parent scope
------------

Parent scope lookup :math:`P(n)` for node :math:`n` is defined as the
first scope node above :math:`n`.

Upwards lookup
--------------

Upwards lookup :math:`U(x_1:: \dots ::x_n)`,
where :math:`x_1:: \dots ::x_n` is identifier,
is defined as list of all declaration-decleration
:math:`(x_1:: \dots ::x_{i-1}, x_i:: \dots ::x_n)` pairs,
s.t. for each pair :math:`x_1:: \dots ::x_{i-1}` is found by
greedly matching beginning of :math:`x_1:: \dots ::x_n` to declerations specified below
in scope nodes :math:`x_1:: \dots ::x_n \circ P \circ \dots \circ P`.

In each scope node if the path to it came from:
    - unordered node:
        - unordered declarations are concidered
    - ordered node :math:`q`:
        - ordered declerations before :math:`q` in reverse order, and
        - unordered declarations are concidered

In the order of discovery is preserved.

Downwards scope lookup
----------------------

Downwards scope lookup :math:`D_s(x_1:: \dots ::x_n, S)`,
where :math:`x_1:: \dots ::x_n` is identifier and :math:`S` is scope node,
is defined as list of all identifier-identifier
:math:`(x_1:: \dots ::x_{i-1}, x_i:: \dots ::x_n)` pairs,
s.t. for each pair :math:`x_1:: \dots :: x_{i - 1}` is the declaration that matched
in :math:`S` and :math:`x_i:: \dots ::x_n` is temporal identifier that is left over.

Downwards lookup
----------------

Downwards lookup :math:`D(x_0:: \dots ::x_n, S)`,
where :math:`x_0:: \dots ::x_n` is indentifier and :math:`S` is scope node,
results in all declerations found by:

1. set :math:`X = x_0:: \dots ::x_n`
2. set :math:`Y = S`
3. for each :math:`(m, l)` in :math:`D_s(X, Y)`:
    - if :math:`l` is empty, mark :math:`m` as found
    - if declaration :math:`d` of :math:`m` is not descendable, break to next :math:`(m, l)` pair
    - set :math:`X = l`
    - set :math:`Y = C(d)`
    - go recursively to step 3

Function name lookup
--------------------

Name lookup :math:`F(x_0:: \dots ::x_n)`,
where :math:`x_0:: \dots ::x_n` is identifier,
results in all declerations found by:

- for all :math:`(m, l)` in :math:`U(x_0:: \dots ::x_n)`:
    - let :math:`m` be the identifier of declaration :math:`d`
    - if :math:`l` is empty, mark :math:`d` found and go to next pair :math:`(m, l)`
    - if :math:`d` is not descentable, go to next pair :math:`(m, l)`
    - mark all declerations found, whome identifier is in :math:`D(l, C(d))`

Non-function name lookup
------------------------

Name lookup :math:`f(x_0:: \dots ::x_n)`,
where :math:`x_0:: \dots ::x_n` is identifier,
is defined as the first decleration found by
function name lookup.
