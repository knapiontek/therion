person:
	p-float: 1.1
	address:
		the-no: 2
		a-float: p-float
	work-address(i: int8): address
		the-i: i

my-person: person
my-address: my-person.work-address(3i8)
------------
y(x: int):
	z: x + 1 << 2
------------
basic:
	person-data(name: unicode, day-of-birth: int):
		.name: name
		.day-of-birth: day-of-birth
	employee-info(name: unicode):
		person-data(name, 36)
		console.out.print-line(.name)
		digit: (37 + (38 * 39))

myself: basic.employee-info("Kris")
------------
person(that: .):
	that.print()
	print(): out.print-line("node1: $.")

person(.)
employee: person(.)
	print(): out.print-line("node2: $. from: $..")
------------
employee(compare(x: int): int):
	compare(6)

plus-seven(x: int): x + 7
plus-seven(x: int): int
	x + 7

myself: employee(plus-seven(int))
-------------
core:
	closure: 0
	memo: core.local.acquire(100 * closure.class.size)
	memo: core.scope.acquire(100)

	person:
		name: unicode
		gender: int

	person(name: unicode, gender: int): person
		.name << name
		.gender << gender
		..closure << 1

	employee-info(name: unicode): person(name, 36)
		gender << 37
		if gender < 3
			log.info("\gender is bigger than 3")
			yield core.execute(x + y)
		else
			log.info("\gender is smaller than 3")
			snap core.execute(x / y)
		while gender <> 0
			gender <- 2
		foreach i: list.end()
			log.info("i: \i")

	execute(operator(x: int, y: int): int): int
		. << 0
		foreach i: list.end()
			. <+ operator(2 /> i.x, 2 /> i.y)
		snap ..

myself: core.employee-info("Kris")

result: core.execute(x * y)
-------------
decimal(float): int
decimal(float) << core.int(float)
five: decimal(5.6)

contstraints(x: float(. < five)): int(x)
	core.assert(x < 5)
-------------
person:
	name: unicode
person(name: unicode): person
	name << name
factory:
	to-person(): $person
factory-impl: factory
	owner: person("John Smith")
	to-person(): $owner
create-factory: $factory
	. << factory-impl.clone()
create-factory: $factory
	. << factory-impl$
core.assert(create-factory$.to-person$.name = "John Smith")
-------------
my-map: scope.map
	pair:
		key: int
		value(x: int): float
-------------
my-map: scope.map(int, result(x: int): float)
	. << (0, 3.0 * x)
	. << (5, float(core.time(x)))

core.assert(nil = my-map[0])
time: my-map[5](2)

person-list: local.list(core.person)
	. << core.person("Kris", 0)
	. << core.person("Aga", 1)
	. << core.person("Jacek", 2) { append }
core.person("Natalia", 3) >> person-list { prepend }
