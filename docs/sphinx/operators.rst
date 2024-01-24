Function calls
==============

Function calls are denoted as:

- identifier
- sematic scope operator: :code:`(`
- potential function arguments
- semantic scope operator: :code:`)`

If the identifier is determined to be function decleration in name lookup,
then function decided by overload resolution is called.
Next if the identifier is determined to be data with:

- function type:
    - that function is called
- class type:
    - function chosen by overload resolution from all :code:`operator()`
      member functions of the type, is called

Lastly if the identifier is class identifier, then the constructor
chosen by overload resolution is called.

Function arguments
------------------

Temporaries passed to functions are valid until end of the expression
in which the function call appeared.

.. Member function calls
.. ---------------------
..
.. Member function calls are denoted as :code:`A.B(...)`:
..
.. - :code:`A`: identifier or temporary data of type :code:`T`
.. - sematic scope operator: :code:`.`
.. - :code:`B`: identifier token
.. - sematic scope operator: :code:`(`
.. - potential function arguments
.. - semantic scope operator: :code:`)`
..
.. Member function calls are equal to: :code:`T::B(A, ...)`.

Operators
=========

Operators are are function calls in expressions with special syntax.
Each operator is turned into spesific function call based on the operands.

Operators are either unary or binary.
Unary operators are all postfix excpet for :code:`!`, :code:`~`, :code:`+` and :code:`-`.
To disambiguate between unary operators and binary operators,
there can not be whitespace iin between identifier token and unary operator,
but for binary operator the whitespace is required.

Order of operations
-------------------

Order in which operations are called is determined by operator precedence (number)
and associativity of the operator (left or right).
See expressions on how these are used.

Parentheses can also change the order of operations.

List of operators
-----------------

In the following table :code:`x` is a identifier of type :code:`T` and :code:`y` is identifier token.

.. list-table:: Unary operators
    :widths: auto
    :header-rows: 1

    * - operator call
      - operator name
      - function call
      - precedence
      - associativity
    * - :code:`+x`
      - unary plus
      - :code:`T::operator+(x)`
      - 1
      - right
    * - :code:`-x`
      - unary minus
      - :code:`T::operator-(x)`
      - 1
      - right
    * - :code:`!x`
      - logical not
      - :code:`T::operator!(x)`
      - 1
      - right
    * - :code:`~x`
      - bitwise not
      - :code:`T::operator~(x)`
      - 1
      - right
    * - :code:`x++`
      - increment
      - :code:`T::operator++(x)`
      - 2
      - left
    * - :code:`x-\-`
      - decrement
      - :code:`T::operator-\-(x)`
      - 2
      - left
    * - :code:`x(...)`
      - function call
      - :code:`T::operator()(x, ...)`
      - 2
      - left
    * - :code:`x[...]`
      - subscript
      - :code:`T::operator[](x, ...)`
      - 2
      - left
    * - :code:`x.y`
      - member access
      - Access data member :code:`y` of :code:`x`
      - 2
      - left
    * - :code:`x&`
      - address-of
      - Address of x
      - 2
      - left
    * - :code:`x*`
      - indirection
      - Dereferene pointer x
      - 2
      - left

In the following table :code:`x` is a identifier of type :code:`T` and :code:`y` is another identifier.

.. list-table:: Binary operators
    :widths: auto
    :header-rows: 1

    * - operator call
      - operator name
      - function call
      - precedence
      - associativity
    * - :code:`x * y`
      - multiplication
      - :code:`T::operator*(x, y)`
      - 3
      - left
    * - :code:`x / y`
      - division
      - :code:`T::operator/(x, y)`
      - 3
      - left
    * - :code:`x % y`
      - moremainder
      - :code:`T::operator%(x, y)`
      - 3
      - left
    * - :code:`x + y`
      - addition
      - :code:`T::operator+(x, y)`
      - 4
      - left
    * - :code:`x - y`
      - subtraction
      - :code:`T::operator-(x, y)`
      - 4
      - left
    * - :code:`x << y`
      - bitwise left shift
      - :code:`T::operator<<(x, y)`
      - 5
      - left
    * - :code:`x >> y`
      - bitwise right shift
      - :code:`T::operator>>(x, y)`
      - 5
      - left
    * - :code:`x <=> y`
      - three-way comparison operator
      - :code:`T::operator<=>(x, y)`
      - 6
      - left
    * - :code:`x < y`
      - less
      - :code:`T::operator<(x, y)`
      - 7
      - left
    * - :code:`x <= y`
      - less or equal
      - :code:`T::operator<=(x, y)`
      - 7
      - left
    * - :code:`x > y`
      - greater
      - :code:`T::operator>(x, y)`
      - 7
      - left
    * - :code:`x >= y`
      - greater or equal
      - :code:`T::operator>=(x, y)`
      - 7
      - left
    * - :code:`x == y`
      - equality
      - :code:`T::operator==(x, y)`
      - 8
      - left
    * - :code:`x != y`
      - quality not
      - :code:`T::operator!=(x, y)`
      - 8
      - left
    * - :code:`x & y`
      - bitwise and
      - :code:`T::operator&(x, y)`
      - 9
      - left
    * - :code:`x ^ y`
      - bitwise xor
      - :code:`T::operator^(x, y)`
      - 10
      - left
    * - :code:`x | y`
      - bitwise or
      - :code:`T::operator|(x, y)`
      - 11
      - left
    * - :code:`x && y`
      - logical and
      - :code:`T::operator&&(x, y)`
      - 12
      - left
    * - :code:`x || y`
      - logical or
      - :code:`T::operator||(x, y)`
      - 13
      - left
    * - :code:`x = y`
      - assignment
      - :code:`T::operator=(x, y)`
      - 14
      - right
    * - :code:`x += y`
      - assignment multiplication
      - :code:`T::operator+=(x, y)`
      - 14
      - right
    * - :code:`x -= y`
      - assignment subtraction
      - :code:`T::operator-=(x, y)`
      - 14
      - right
    * - :code:`x *= y`
      - assignment multiplication
      - :code:`T::operator*=(x, y)`
      - 14
      - right
    * - :code:`x /= y`
      - assignment division
      - :code:`T::operator/=(x, y)`
      - 14
      - right
    * - :code:`x %= y`
      - assignment remainder
      - :code:`T::operator%=(x, y)`
      - 14
      - right
    * - :code:`x <<= y`
      - assignment bitwise left shift
      - :code:`T::operator<<=(x, y)`
      - 14
      - right
    * - :code:`x >>= y`
      - assignment bitwise right shift
      - :code:`T::operator>>=(x, y)`
      - 14
      - right
    * - :code:`x &= y`
      - assignment bitwise and
      - :code:`T::operator&=(x, y)`
      - 14
      - right
    * - :code:`x ^= y`
      - assignment bitwise xor
      - :code:`T::operator^=(x, y)`
      - 14
      - right
    * - :code:`x |= y`
      - assignment bitwise or
      - :code:`T::operator|=(x, y)`
      - 14
      - right
