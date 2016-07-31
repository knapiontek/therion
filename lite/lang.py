
string: ''
string: u8[]
int: i32

# single line comment

{
    multi-line comment
}

address-town: (town: string)
address: address-town (street: string, house-no: int)
address-ref: address
my-address: address (town: 'my-town', street: 'my-street', house-no: 1)
your-address: address ('your-town', 'your-street', 2)
their-address: address
   town: 'my-town'
   street: 'my-street'
   house-no: 3

family:
    last-name: string,
    first-name-seq: string[],
    address-seq: address[]

family-seq: family[]

family-seq << family
    'Smith',
    ['John', 'Mark', 'Teddy', 'Betty'],
    [(street: 'Avenue'), (street: 'Strand')]

item: (street: address.street, house-no: address.house-no, family.last-name)

count: 0i16
find-by-name: (street: address.street) -> item[]
    for f in family-seq[f.last-name = 'Smith' & f.first-name-seq[name]]
        for a in f.address-seq[a.street = street]
            if [count < 100]
                out << item (a.street, a.house-no, f.last-name)
                count ++
            for [count > 100]
                count --

hello: item[]
find-by-name ('Quay') -> hello
