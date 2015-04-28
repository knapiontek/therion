find . -name *.hpp|xargs perl -pi -e 's/\s+$/\n/g'
find . -name *.cpp|xargs perl -pi -e 's/\s+$/\n/g'
find . -name *.py|xargs perl -pi -e 's/\s+$/\n/g'

