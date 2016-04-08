```javascript

address { street: unicode, house-no: int }

family-seq {
    last-name: unicode,
    first-name-seq: unicode[]
    address-seq: address[]
} []

family-seq << { 'Napiontek', ['Kris', 'Aga', 'Borys', 'Sophia'], [ { street: 'Southdene' }, { street: 'Warszawska' } ] }

family-name-seq: family-seq.last-name

find-by-name(name: unicode): family-seq[last-name = 'Napiontek' & first-name-seq[.name] = nil] {
    name: last-name,
    street: address-seq[0]
} []

test: find-by-name('Hive')

```
