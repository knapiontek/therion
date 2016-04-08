```javascript

address { street: unicode, house-no: int }
factory-address: address { city: unicode }

family {
    last-name: unicode,
    first-name-seq: unicode[]
    address-seq: address[]
}

family-seq: family[]
my-family: family { 'Napiontek', ['Kris', 'Aga', 'Borys', 'Sophia'], [ { street: 'Southdene' }, { street: 'Warszawska' } ] }
family-seq << my-family

family-name-seq: family-seq.last-name

find-by-name(name: unicode) {
    result: family-seq[last-name = 'Napiontek' & first-name-seq[.name] = nil].address-seq
}

my-result(my-address: address) {
    my-street: address.street,
    my-house-no: address.house-no
}

test: my-result(find-by-name('Hive'))

```
