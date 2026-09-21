# langs
Language Projects Support Files

Under Construction, completing Summer 2027

New github created primarily to view my .md format document files.

----------

**mm.ma** MM compiler source code in amalgamated form. View using 4-space tabs. It can be split up into discrete files using a script as the format is simple (a "===" line precedes each file).

Or, if the MM compiler is running under Windows, using 'mm -deconst mm.ma'

Note this may contain lots of junk such as commented-out debug statements.

**mm7.hex** This is the binary for the MM compile for windows (mm7.exe). It is in hex text format to get around AV issues. Another very simple script can convert it to binary: convert each two characters representint a hex value and write as an 8-bit byte (so the two-byte "41" needs to be written as the single 0x41 byte).

**mm.c** In case you're on Linux, M can be transpiled to poor quality C (using an M compiler with a special backend, not uploaded). This was applied to the Windows M compiler. Build on Linux using the instructions at the top. (Note this is 100Kloc file of 2.3MB.)

This is limited in what it can do, as it still targets Windows, and may need access to certain DLLs even to generate EXEs. However the following options may work:
````
  ./mm -a prog             Compile prog.m to assembly (prog.asm)
  ./mm -p prog             Compile to IL (prog.pcl)
  ./mm -i prog             Interpret the generated IL (so does not produce native code)
````
The ASM output is normally for my x64 assembly. But I see that it was configured to use GAS format for some test, as I found out after uploading. So it will be GAS but for Win64 ABI - it may assemble, but you can't run it!

The -deconst option won't work here as it invokes the "md" shell command which is for Windows.

If trying to use this on mm.ma, then './mm -a' won't work some reason to do with the 'power' operator. But './mm -p mm.ma' seems to work.

'./mm -i mm.ma' doesn't work either, but the interpreter option has always been sketchy. It might be OK for example programs though. And it can run bignum.m:
````
  ./mm -i bignum
````
(I've re-uploaded bignum.m to remove inline assembly. This library, invoked by itself, will run a *pi* calculation demo. The library is slow and the very slow interpreter here doesn't help.)

