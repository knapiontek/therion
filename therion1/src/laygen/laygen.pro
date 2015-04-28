
Model{class} : Element+ $00

Element{class}
: Ln ElementId $s:$s TypeId Params?
: Ln TypeId Params?
: Ln

Params{class} : ( String )

Ln{class} : $n Tab*
Tab{byte} : $t

String{byte-list} : " MiddleChar+ "

ElementId{byte-list} : Id
TypeId{byte-list} : Id

Id{byte-list} : FirstChar MiddleChar+ LastChar

FirstChar{byte}
: Upper
: Lower

MiddleChar{byte}
: Upper
: Lower
: Digit
: -

LastChar{byte}
: Upper
: Lower
: Digit

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

