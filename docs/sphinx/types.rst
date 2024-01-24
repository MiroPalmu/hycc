Types
=====

Type system is subset of C++ type system with influence from
Herb Stutter's cpp2 project.

- Fundamental types:
    - :code:`void`
    - Arithmetic types:
        - Ingeteral types:
            - :code:`bool`
            - :code:`char`, :code:`uchar`
            - :code:`short`, :code:`ushort`
            - :code:`int`, :code:`uint`
            - :code:`long`, :code:`ulong`
            - :code:`longlong`, :code:`ulonglong`
        - Floating-point types:
            - :code:`f32`
            - :code:`f64`
- Compound types:
    - Pointer types:
        - pointer-to-object types
    - Function types
    - Class types

For each type other than refernce and function, the type system supports
also supports const-qualified version of it, e.g. for type :code:`T`
const-qualified version is :code:`const T`. Multiple const-qualifiers
are truncated to one const.

Size
----

Each type :code:`T` has width which can be queried with :code:`sizeof(T)`
intrinsic function call. This returns the size of the type in
bytes (note that byte is implementation defined concept).

Fundamental types
=================

Fundamental types are special in sense that for fundamental type :code:`T`
following member functions are defnied:

.. code-block::

    operator++: () -> T;
    operator--: () -> T;

    operator*: (rhs: T) -> T;
    operator/: (rhs: T) -> T;
    operator%: (rhs: T) -> T;

    operator+: (rhs: T) -> T;
    operator-: (rhs: T) -> T;

    operator<=>: (rhs: T) -> T;

    operator&: (rhs: T) -> T;
    operator^: (rhs: T) -> T;
    operator|: (rhs: T) -> T;

:code:`bool` different because only following member functions are defined for it:

.. code-block::

    operator==: (rhs: bool) -> bool;

    operator&: (rhs: bool) -> bool;
    operator^: (rhs: bool) -> bool;
    operator|: (rhs: bool) -> bool;

    operator&&: (rhs: bool) -> bool;
    operator^^: (rhs: bool) -> bool;
    operator||: (rhs: bool) -> bool;

void
----

Incomplete type, so can not be constructed.
It is a type with empty set of possible values.

Integral types
--------------

Each integer type has data model dependent width
with additional guarantee that there is minimum widths
and following is always true:

.. math::

   1 == sizeof(char) <= sizeof(short) <= sizeof(int) <= sizeof(long) <= sizeof(longlong)

.. list-table:: Integral types
   :widths: auto
   :header-rows: 1

   * - type
     - minimum width in bits
   * - :code:`char`
     - 8
   * - :code:`short`
     - 16
   * - :code:`int`
     - 16
   * - :code:`long`
     - 32
   * - :code:`longlong`
     - 64

Each integer are signed by default. They can be made unsigned
by adding :code:`u` prefix: :code:`uchar`, `ushort`, :code:`uint`, `ulong`, :code:`ulonglong`.

:code:`sizeof(T)` return type is :code:`size_t`, which is defined to be unsigned integral
wide enough, that every possible address can fit into it.

Floating-point types
--------------------

.. list-table:: Floating-point types
   :widths: auto
   :header-rows: 1

   * - type
     - format
   * - :code:`f32`
     - IEEE-754 binary32
   * - :code:`f64`
     - IEEE-754 binary64

Compound types
==============

Pointer types
-------------

Pointer types store address of a object. Pointer to type :code::code:`T` is :code:`*T`.
They can be obtained from using postfix :code:`operator&: () -> *T` operator.
Width of pointer type is always the same as :code:`size_t`.

Function types
--------------

Function types are similar to pointer but they point to functions.
Function taking parameters :code:`T1, T2, ..., Tn` returning a type :code:`R`
has type :code:`(T1, T2, ..., Tn) -> R`.
Width of object whose type is function type is the same as for pointer.

Defining aspect of function type is that they can be called using
:code:`operator(): (arg1: T1, arg2: T2, ..., arg3: Tn) -> R`.

There is special annotations for types of parameter passing.
Passing annotation for parameter :code:`argi` is written before the name
and it defines how to parameter is given to the function.
Types are:

- :code:`in` - default, read-only, pass by by value or by pointer.
- :code:`inout` - pass by mutable pointer.
- :code:`out` - pass by pointer which has to be written to in the function body and can not be used before.
- :code:`move` - same as :code:`inout` but argument do not have to be in valid state after.
- :code:`copy` - argument is copied from.
- :code:`forward` - forwards argument to other function as if the forwarding never happened.

If argument is passed by pointer, then dereferencing it happens automatically
in the function body whenever the argument is used.

.. code-block::

   foo: (arg: int) -> void = {
       bar: = arg;
       // error:
       // arg = bar;
   }

   foo: (inout arg: int) -> void = {
       bar: = arg;
       arg = bar;
   }

   foo: (out arg: int) -> void = {
       // error:
       // bar: = arg;
       arg = bar;
       // ok:
       bar: = arg;
   }

   foo: (move arg: int) -> void = {
       // move
       bar: = arg;
       // Usage arg is UB
   }

   foo: (forward arg: int) -> void = {
       // error:
       bar: = arg;
       // as if foo function call never happened in point of view of arg:
       use_arg(arg);
   }

Class types
-----------

Class types are aggregates of other types. They can have data and function
members. They are declared using intrinsic type called :code:`type`:

.. code-block::

    my_class: type { ... };

Class scope is the scope inside brackets of class decleration.
Class scope can only contain variable declerations and function declerations
which define data member and function member respectively.

Function members have a restriction that their first argument
has to be named :code:`this` and it has a implicitly give the type of the class.
Function members are called with following syntax :code:`object.member_function(...)`
but with special rule
that the object itself is passed implicitly as the :code:`this` argument.
There is no way to specify :code:`this` argument explicitly.

Data members follow normal variable decleration. If they are also defined
at the same time then they are given default value which is constructed when
object of that class is constructed. Default values can only contain
compile time data.

Data members can be accessed with following syntax :code:`object.data_member`.
They can be used inside member functions using just the name.
Dereference of :code:`this` is added implicitly.

Declerations of data :code:`foo: T = bar` are equivalent to :code:`foo: T; foo = bar`.
Calling class identifier as function :code:`T(...)` is equivalent to
declaring data of that type and calling `T::operator=: (out, ...) -> void` on it.
When data of type :code:`T` goes out of scope :code:`T::operator=: (move this) -> void`
is called on it.

.. code-block::

    my_class: type {
        data1: int = 2;
        data2: float = 3.0;

        foo1: (this, a: int) -> int = { return data1 + a; }
        foo2: (this) -> int = {
            // Can not modify data
            // data1 = 1;
        }
        foo3: (inout this) -> int = {
            // Can modify data
            data1 = 1;
        }

    };


    obj: my_class;
    use_float(obj.data2);

    obj_const: const my_class;
    // ok
    obj_const.foo2();
    // not ok, can not modify const object
    // obj_const.foo3();

    obj_mutable: my_class;
    // ok
    obj_const.foo2();
    // ok
    obj_const.foo3();
