```javascript
string: ''
unicode: u8[]
int: i32

// address

/*
    multi-line comment
*/

address-town: { town: unicode }
address: address-town { street: unicode, house-no: int }
address-ref: address
my-address: address { town: 'my-town', street: 'my-street', house-no: 1 }
your-address: address { 'your-town', 'your-street', 2 }

family: {
    last-name: unicode,
    first-name-seq: unicode[],
    address-seq: address[]
}

family-seq: family[]

family-seq << family {
    'Smith',
    ['John', 'Mark', 'Teddy', 'Betty'],
    [ { street: 'Avenue' }, { street: 'Strand' } ]
}

item: { street: address.street, house-no: address.house-no, family.last-name }

find-by-name(street: address.street):
    family-seq[.last-name = 'Napiontek' & .first-name-seq[name]]
        .address-seq[.street = street]
            item { .street, .house-no, ..last-name }
```
