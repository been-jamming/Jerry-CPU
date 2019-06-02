# Jerry-CPU
Minecraft CPU and development tools

This repository currently includes an assembler, emulator, and WIP C compiler for a CPU I made in minecraft which I named Jerry. Besides targeting the C compiler for other architectures, this project is purely for my enjoyment. 

You can use `make` to build the assembler and emulator. If you want to test my multiply program, first type the following command to assemble the program:```jasm multiply multiply.o```

This command will output a "binary" file named `multiply.o`. For my convenience, the file isn't truely a binary file, but an annotated text file with 1s and 0s. I did this so that I could program the CPU easily by hand with the assembler output since I don't have a way yet to do this automatically.

To run the program, next type ```emulator multiply.o```. The program should ask you for two input numbers, and then the result should be stored at the end in register 2. The input numbers should be both positive and result in an answer which can fit in an unsigned 8 bit number, otherwise the CPU will show incorrect results.

If you would like to contribute to creating mod or datapack which automatically places torches in minecraft to program the CPU, message me on discord (`megaman#1015`)
