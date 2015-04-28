
Model{class}
: Angle
: Quote

Angle{class} : #include$s< Path >
Quote{class} : #include$s" Path "

Path{class} : FolderPath / File

FolderPath{class}
: FolderPath / Folder
: Folder

Folder{class} : Alpha+

File{class} : Alpha+ Ext

Ext{class}
: .h
: .hpp

Alpha{byte}
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

