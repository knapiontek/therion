scala> List(1,2)
res0: List[Int] = List(1, 2)

scala> List.range(1, 5)
res1: List[Int] = List(1, 2, 3, 4)

scala> List.range(1, 5).map(x => x * x)
res2: List[Int] = List(1, 4, 9, 16)

scala> List.range(1, 5).map(x => x * x)
res3: List[Int] = List(1, 4, 9, 16)

scala> List.range(1, 5).map(x => if(x > 2) Some(x * x) else None)
res4: List[Option[Int]] = List(None, None, Some(9), Some(16))

scala> def g(v:Int) = List(v-1, v, v+1)
g: (v: Int)List[Int]

scala> def g = (v:Int) => List(v-1, v, v+1)
g: Int => List[Int]

scala> g(1)
res5: List[Int] = List(0, 1, 2)

scala> g(11)
res6: List[Int] = List(10, 11, 12)
