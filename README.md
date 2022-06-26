# Haffman
Not an optimal Huffman compression code

1 The first step is to read the data from the file and count which character appeared and with what frequency in the freq array.

2 Then we build a code tree, where we encode the most frequently appearing character with the minimum number of bits. 
Each character is represented by a set of 0 and 1 depending on the frequency.

3 Using union, we combine the bits of the code into a new character, consisting of 8 bits, in ASCII encoding

4 The last step is to write the compressed text back to the file, along with the decompression table. Each character comes with its own code

Decompression occurs in reverse order
1 Character codes are read, stored in a separate array.

2 The text is being read.

3 For each element from the text in the table there is an ASCII code, a new line of 0 and 1 is compiled.

4 It goes through a new line and looks if there is such a symbol in the code table, if not, it increases by the next character.

5 Writes to a file.


For a 170 KB file, compression takes 22 seconds, decompression takes 17
