```javascript

address: { street: unicode, house-no: int }

address-seq: address[]

factory:
{
        name: unicode,
        hq-address: address { street: first, house-no: 1 }
}

family:
{
        last-name: unicode,
        first-name-seq: unicode[]
}

my-family: family { 'Napiontek', ['Kris', 'Aga', 'Borys', 'Sophia'] }

family-seq: [ my-family ]

family-name-seq: family-seq.last-name

result: family-name-seq(family-seq[last-name = 'Napiontek' & first-name-seq['Rex'] = nil]).last-name

```
