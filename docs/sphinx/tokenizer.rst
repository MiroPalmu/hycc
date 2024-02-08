.. _chars:

Characters
==========

Tokenizer parses a UTF-8 encoded text.
The source code can contain characters
only from `C++26 basic character set`_,
except literals do not have this restriction.

.. _`C++26 basic character set`: https://en.cppreference.com/w/cpp/language/charset

Character classes
-----------------

Hycc classifies characters in C++26 basic character set
into character classes:

- whitespace
- id
- integer
- literal scope operator
- semantic scope operator
- operator unit


.. list-table:: whitespace
    :widths: auto
    :header-rows: 1

    * - Code unit
      - Character
      - Glyph
    * - U+0009
      - Character tabulation
      -
    * - U+000B
      - Line tabulation
      -
    * - U+000C
      - Form feed (FF)
      -
    * - U+0020
      - Space
      -
    * - U+000A
      - Line feed (LF)
      - new-line

.. list-table:: id
    :widths: auto
    :header-rows: 1

    * - Code unit
      - Character
      - Glyph
    * - U+0041 .. U+005A
      - Latin capital letter
      - :code:`A`, ..., :code:`Z`
    * - U+005F
      - Low line
      - :code:`_`
    * - U+0061 .. U+007A
      - Latin small letter
      - :code:`a`, ..., :code:`z`

.. list-table:: integer
    :widths: auto
    :header-rows: 1

    * - Code unit
      - Character
      - Glyph
    * - U+0030 .. U+0039
      - Digit
      - :code:`0`, ..., :code:`9`

..  list-table:: literal scope operator
    :widths: auto
    :header-rows: 1

    * - Code unit
      - Character
      - Glyph
    * - U+0022
      - Quotation mark
      - :code:`"`
    * - U+0027
      - Apostrophe
      - :code:`'`
    * - U+0060
      - Grave Accent
      - :code:`\``

.. list-table:: semantic scope operator
    :widths: auto
    :header-rows: 1

    * - Code unit
      - Character
      - Glyph
    * - U+0028
      - Left parenthesis
      - :code:`(`
    * - U+0029
      - Right parenthesi
      - :code:`)`
    * - U+002C
      - Comma
      - :code:`,`
    * - U+002E
      - Full sto
      - :code:`.`
    * - U+003A
      - Colo
      - :code:`:`
    * - U+003B
      - Semicolo
      - :code:`;`
    * - U+005B
      - Left square bracket
      - :code:`[`
    * - U+005D
      - Right square bracket
      - :code:`]`
    * - U+007B
      - Left curly bracket
      - :code:`{`
    * - U+007D
      - Right curly bracket
      - :code:`}`

.. list-table:: operator unit
    :widths: auto
    :header-rows: 1

    * - Code unit
      - Character
      - Glyph
    * - U+0021
      - Exclamation mark
      - :code:`!`
    * - U+0023
      - Number sign
      - :code:`#`
    * - U+0025
      - Percent sign
      - :code:`%`
    * - U+0026
      - Ampersan
      - :code:`&`
    * - U+002B
      - Plus sign
      - :code:`+`
    * - U+002D
      - Hyphen-minus
      - :code:`-`
    * - U+002F
      - Solidus
      - :code:`/`
    * - U+003C
      - Less-than sign
      - :code:`<`
    * - U+003D
      - Equals sign
      - :code:`=`
    * - U+003E
      - Greater-than sign
      - :code:`>`
    * - U+003F
      - Question mark
      - :code:`?`
    * - U+005C
      - Reverse solidus
      - :code:`\\`
    * - U+005E
      - Circumflex accent
      - :code:`^`
    * - U+007C
      - Vertical line
      - :code:`|`
    * - U+007E
      - Tilde
      - :code:`~`
    * - U+0024
      - Dollar Sign
      - :code:`$`
    * - U+0040
      - Commercial At
      - :code:`@`

In other words:

.. list-table:: Character classes
    :widths: auto
    :header-rows: 1

    * - Character class
      - Code unit
      - Character
      - Glyph
    * - whitespace
      - U+0009
      - Character tabulation
      -
    * - whitespace
      - U+000B
      - Line tabulation
      -
    * - whitespace
      - U+000C
      - Form feed (FF)
      -
    * - whitespace
      - U+0020
      - Space
      -
    * - whitespace
      - U+000A
      - Line feed (LF)
      - new-line
    * - operator unit
      - U+0021
      - Exclamation mark
      - :code:`!`
    * - literal scope operator
      - U+0022
      - Quotation mark
      - :code:`"`
    * - operator unit
      - U+0023
      - Number sign
      - :code:`#`
    * - operator unit
      - U+0025
      - Percent sign
      - :code:`%`
    * - operator unit
      - U+0026
      - Ampersan
      - :code:`&`
    * - literator scope operator
      - U+0027
      - Apostrophe
      - :code:`'`
    * - semantic scope operator
      - U+0028
      - Left parenthesis
      - :code:`(`
    * - semantic scope operator
      - U+0029
      - Right parenthesi
      - :code:`)`
    * - operators unit
      - U+002A
      - Asterisk
      - :code:`*`
    * - operator unit
      - U+002B
      - Plus sign
      - :code:`+`
    * - semantic scope operator
      - U+002C
      - Comma
      - :code:`,`
    * - operator unit
      - U+002D
      - Hyphen-minus
      - :code:`-`
    * - semantic scope operator
      - U+002E
      - Full sto
      - :code:`.`
    * - operator unit
      - U+002F
      - Solidu
      - :code:`/`
    * - number
      - U+0030 .. U+0039
      - Digit
      - :code:`0`, ..., 9
    * - semantic scope operator
      - U+003A
      - Colo
      - :code:`:`
    * - semantic scope operator
      - U+003B
      - Semicolo
      - :code:`;`
    * - operator unit
      - U+003C
      - Less-than sign
      - :code:`<`
    * - operator unit
      - U+003D
      - Equals sign
      - :code:`=`
    * - operator unit
      - U+003E
      - Greater-than sign
      - :code:`>`
    * - operator unit
      - U+003F
      - Question mark
      - :code:`?`
    * - id
      - U+0041 .. U+005A
      - Latin capital letter
      - :code:`A`, ..., :code:`Z`
    * - semantic scope operator
      - U+005B
      - Left square bracket
      - :code:`[`
    * - operator unit
      - U+005C
      - Reverse solidus
      - :code:`\\`
    * - semantic scope operator
      - U+005D
      - Right square bracket
      - :code:`]`
    * - operator unit
      - U+005E
      - Circumflex accent
      - :code:`^`
    * - id
      - U+005F
      - Low line
      - :code:`_`
    * - id
      - U+0061 .. U+007A
      - Latin small letter
      - :code:`a`, ..., :code:`z`
    * - semantic scope operator
      - U+007B
      - Left curly bracket
      - :code:`{`
    * - operator unit
      - U+007C
      - Vertical line
      - :code:`|`
    * - semantic scope operator
      - U+007D
      - Right curly bracket
      - :code:`}`
    * - operator unit
      - U+007E
      - Tilde
      - :code:`~`
    * - operator unit
      - U+0024
      - Dollar Sign
      - :code:`$`
    * - operator unit
      - U+0040
      - Commercial At
      - :code:`@`
    * - literator scope operator
      - U+0060
      - Grave Accent
      - :code:`\``

Tokens
======

Source code is split to different tokens.
Tokens are constructed form the characters
based on token types.

Token types
-----------

Token types are defined in table below,
using following format:

- :code:`x` means one character from class :code:`x`
- :code:`x|y` means :code:`x` or :code:`y`
- :code:`<x>` means one or more characters form class :code:`x`
- :code:`[x]` means any number of characters form class :code:`x`
- :code:`...` means any number of arbitrary characters
- :code:`c = y` defines :code:`c` as character from class :code:`y`

.. list-table:: Token types
    :widths: auto
    :header-rows: 1

    * - type
      - additional info
      - format
    * - whitespace token
      -
      - :code:`<whitespace>`
    * - identifier token
      -
      - :code:`<id>[id|number]`
    * - integer token
      -
      - :code:`<integer>`
    * - literal token
      - :code:`L` = literal scope operator
      - :code:`L...L`
    * - semantic scope operator
      -
      - :code:`semantic scope operator`
    * - operator token
      -
      - :code:`<operator unit>`
    * - error token
      - define class :code:`X` containing every character
      - :code:`X`

Tokenizer
---------

Tokenizer takes string of :ref:`characters <chars>`
as a input and produces a array of tokens,
with following greedy algorithm:

1. Set pointer :code:`A` to beginning of input.
2. Identify token starting at :code:`A`:
    - each token class is concidered one by one in order:
        1. whitespace
        2. integer token
        3. literal token
        4. semantic scope operator
        5. operator token
        6. identifier token
        7. error token
3. In case of error class report it and stop.
4. Set pointer :code:`B` to one past end of indentified token.
5. Store token :code:`[A,B)` with metadata.
6. If :code:`B` is one past end of input, then stop.
7. Set :code:`A` to :code:`B` and go to step 1.
