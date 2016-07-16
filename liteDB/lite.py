
string: ''
string: u8[]
int: i32

# single line comment

{*
    multi-line comment
 *}

address-town: { town: string }
address: address-town { street: string, house-no: int }
address-ref: address
my-address: address { town: 'my-town', street: 'my-street', house-no: 1 }
your-address: address { 'your-town', 'your-street', 2 }
their-address: address
   town: 'their-town'
   street: 'their-street'
   house-no: 3

family:
    last-name: string
    first-name-seq: string[]
    address-seq: address[]

family-seq: family[]

family-seq << family
    'Smith'
    ['John', 'Mark', 'Teddy', 'Betty']
    [{ street: 'Avenue' }, { street: 'Strand' }]

simple-function: { x: i32, y: i32 } -> x + y

item: { street: address.street, house-no: address.house-no, family.last-name }

find-by-name: { street: address.street, name: string } ->
    family-seq[last-name = 'Smith' & first-name-seq[name]]
            .address-seq[street = street]
        item { street, house-no, last-name }
