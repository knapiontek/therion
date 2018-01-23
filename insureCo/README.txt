Python 3.5.2
> python3 ./insureco.py
> python3 ./test.py

I have decided to create python script with no extra tools and libs for simplicity
I achieved scaling using CPU cores as follows:
 * losses.csv can be of an arbitrary length, I am using generators
 * deals.csv - in real scenario should be kept in distributed database (i.e. MongoDB) and available for all machines running DataMap()
 * DataMap() can be run in parallel on any number of machines
 * real scenario would require a message queue (i.e. RabbitMQ) instead of implicit multiprocessing.Queue in multiprocessing.Pool
 * scaling with Pool.apply_async() should be replaced by a cluster with an application server (i.e. Flask, DJango)
 * DataReduce() is not scaled well in this script. It would look very artificial and dirty.
   In real scenario chunks of data can be reduced locally and send to a message queue for further processing.
   I am using only one level of map-reduce but my idea can be transformed into a tree of arbitrary height.
