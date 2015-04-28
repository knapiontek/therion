
# $ - special token

input: ClassMap FuncMap

ClassMap : ClassMap Class
ClassMap : Class

Class : id { DeclMap }

DeclMap : DeclMap Decl
DeclMap : Decl

Decl : key Type id ;
Decl : unique Type id ;
Decl : Type id ;

FuncMap : FuncMap Func
FuncMap : Func

Func : Type id ( ArgMap ) { }

ArgMap : ArgMap , Arg
ArgMap : Arg

Arg : Type $id

Type : Type [ ]
Type : Type [ decimal ]
Type : bool
Type : int8
Type : int16
Type : int32
Type : int64
Type : int128
Type : uint8
Type : uint16
Type : uint32
Type : uint64
Type : uint128
Type : float32
Type : float64
Type : float128
Type : $id $
Type : ( ) . | = <> <= < >= > & float

Expr : Expr  Expr
Expr : Expr / Expr
Expr : Expr  Expr
Expr : Expr - Expr
Expr : Expr & Expr
Expr : Expr | Expr
Expr : $decimal
Expr : $float
Expr : $id
Expr : $string

