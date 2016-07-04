```javascript

string: ''
unicode: u8[]
int: i32

// address

/*
    multi-line comment
*/

address-town: (town: unicode)
address: address-town (street: unicode, house-no: int)
address-ref: address
my-address: address (town: 'my-town', street: 'my-street', house-no: 'my-house-no')
address (town: unicode, street: unicode, house-no: int): address {
    .town: town
    .street: street
    .house-no: house-no
}
your-address: address ('your-town', 'your-street', 'your-house-no')

family: (
    last-name: unicode,
    first-name-seq: unicode[],
    address-seq: address[]
)

family-seq: family[]

family-seq << family (
    'Napiontek',
    ['Kris', 'Aga', 'Borys', 'Sophia'],
    [ (street: 'Southdene'), (street: 'Warszawska')]
)

item: (
    street: address.street,
    house-no: address.house-no,
    family-name: family.last-name
)

find-by-name (name: unicode): item[] {
    items: family-seq[.last-name = 'Napiontek' & first-name-seq[name]].address-seq[street = 'Southdene']
    for (i: items) { . << item (street: i.street, house-no: i.house-no, family-name: name) }
}

hive-seq: find-by-name ('Hive')

person: (name: unicode, address: (street: unicode))

employee: person (work: unicode)

find-people (name: unicode): person[] {
    test: unicode
    if (test = name) {
        . << person[]
    }
}

hello: find-people ("Kris")

find-address (name: unicode): person[] {
    for (i: people[name = "Kris"]) {
        log.debug << i
        . << i.address
    }
}

find-address ('John') >> hello

test (name: unicode, lambda (name: unicode): unicode): lambda (name)
```
