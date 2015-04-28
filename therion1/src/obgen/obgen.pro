
Model{class} : Clazz+ Global+ $00

Clazz{class} : Ln ClazzId Ln { Field+ Ln } Ln

Field{class}
: Ln TypeId $s VarId
: Ln TypeId $$$s VarId
: Ln TypeId []$s VarId
: Ln

Global{class}
: Ln TypeId []$s VarId
: Ln @$s Index
: Ln @@$s Index
: Ln Function
: Ln

Function{class}
: TypeId $s FunctionId () Ln { Ln @$s Index Ln }
: TypeId $s FunctionId () Ln { Ln @@$s Index Ln }
: TypeId $s FunctionId ( Params ) Ln { Ln return$s VarChain [ Condition ] Ln }

Index{class} : VarId Square+

Square{class} : [ VarChain ]

Params{class-list}
: Params ,$s Param
: Param

Param{class} : TypeId $s VarId

Condition{class}
: ( Condition )
: Condition $s Op $s Value
: Value

ClazzId{byte-list} : Id
TypeId{byte-list} : Id
VarId{byte-list} : Id
FunctionId{byte-list} : Id

Ln{} : $n Tab*
Tab{byte} : $t

Op{byte-list}
: =
: <>
: <
: <=
: >
: >=
: *
: /
: %
: +
: -
: &
: |

Value{class}
: VarChain
: String
: Float
: Decimal

VarChain{class-list}
: VarChain . VarId
: VarId

String{byte-list} : " Next+ "

Float{class}
: Decimal . Digit* E Decimal
: Decimal . Digit*

Decimal{class} : Sign? Digit+

Id{byte-list} : First Next+

First{byte}
: Upper
: Lower
: _

Next{byte}
: Upper
: Lower
: Digit
: _

Sign{byte}
: +
: -

Upper{byte}
: A
: B
: C
: D
: E
: F
: G
: H
: I
: J
: K
: L
: M
: N
: O
: P
: Q
: R
: S
: T
: U
: V
: W
: X
: Y
: Z

Lower{byte}
: a
: b
: c
: d
: e
: f
: g
: h
: i
: j
: k
: l
: m
: n
: o
: p
: q
: r
: s
: t
: u
: v
: w
: x
: y
: z

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

