```

string: ''
unicode: u8[]
int: i32

address: (street: unicode, house-no: int)

family: (
    last-name: unicode,
    first-name-seq: unicode[],
    address-seq: address[]
)

family-seq: family[]

family-seq << family (
    'Napiontek',
    ['Kris', 'Aga', 'Borys', 'Sophia'],
    [ (street: 'Southdene'), (street: 'Warszawska') ]
)

item: (
    street: address.street,
    house-no: address.house-no,
    family-name: family.last-name
)

find-by-name(name: unicode): item[] {
    items: family-seq[last-name = 'Napiontek' & first-name-seq[name]].address-seq[street = 'Southdene']
    for i: items { . << (street: i.street, house-no: i.house-no, family-name: name) }
}

hive-seq: find-by-name('Hive')

person: (name: unicode, address: (street: unicode))

employee: person(work: unicode)

find-people(name: unicode): person[] {
    test: unicode
    if test = name {
        . << person[]
    }
}

hello: find-people("Kris")

find-address(name: unicode): person[] {
    for i: people[name = "Kris"] {
        . << i.address
    }
}

find-address('John') >> hello

lambda-test((name: unicode): unicode)

```
