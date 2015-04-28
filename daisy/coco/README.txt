 * Coco.exe compilation:

1. download source of cocoR
2. remove ' ' checkup from Parser.cs (search for 'blanks')
3. gmcs *.cs will compile Coco.exe
4. copy Coco.exe and *.frame files to install directory
5. replace ' ' with '\xff' in Scanner.frame file in install directory

original Coco.exe compilation requires blanks skipped but our grammar takes care of blanks

 * Error handling:

- var not used
