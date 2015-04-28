employ:
	that-j: 1
address: employ
	that-i: 2
the-j: address.that-j
=====================
person:
	p-float: 1.1
	agent:
		employ:
			a-float: p-float
		employ(j: int8): employ
			that-j: j
	proxy:
		address(i: int8): agent.employ(i)
			a-float: p-float
			that-i: i

the-address: person.proxy.address(3i8)
the-i: the-address.that-i
the-j: person.proxy.address(4i8).that-j
=======================================
person:
	p-float: 1.1
	employ:
		a-float: p-float
	address(i: int8): employ
		a-float << p-float
		that-i: i

the-address: person.address(3i8)
the-i: the-address.that-i
the-i << person.address(4i8).that-i

five(i: int): 5

copy: five$(int)
copy(3)

call(i: int): int
call$(int) << five$(int)
call(4)

val(): $the-i * 2
val: val()$

home(h: int):
	hh: h
town(t: int): home(t)
	tt: t
result: town(1)

$float(i: int):
	. << $i

<< (p1: $person, p2: $person):
	p1$.name << p2$.name
	p1$.address << p2$.address
