
Model{class} : Main Decimal? $00

Main{class} : Terminal FloatList Terminal

Terminal{class}
: $097$5E
: x$$$20$$
: Zero a
: Unique
: << ZeroUnique >>

ZeroUnique{byte}
: Zero
: Unique

Zero{byte}
: K
: $00

Unique{byte}
: L
: K

FloatList{class}
: FloatList , Float
: Float

Float{class}
: Decimal . Digit* Exp Decimal
: Decimal . Digit*

Decimal{class} : Optional Sign? Digit+ Optional

Optional{class}
: *
: %
:

Sign{byte}
: +
: -

Digit{byte}
: 0
: 1
: 2
: 3
: 4
: 5
: 6
: 7
: 8
: 9

Exp{byte}
: E
: e
