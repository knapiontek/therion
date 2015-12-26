#generate svg pages from dvi format

cd D:\kris\therioncode\math\fem-article
D:\programs\dvisvgm.exe -p 1-16 -b A4 --libgs="C:\Program Files\gs\gs9.14\bin\gsdll64.dll" fem.dvi -o page/%f-%p.svg

cd C:\Kris\therioncode\math\fem-article
C:\programs\dvisvgm.exe -p 1-16 -b A4 --libgs="C:\Program Files\ghostscript\bin\gsdll64.dll" fem.dvi -o page/%f-%p.svg


"C:\Program Files (x86)\MiKTeX 2.9\miktex\bin\dvipng.exe" fem.dvi