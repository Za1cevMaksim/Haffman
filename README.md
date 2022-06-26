# Haffman
Not an optimal Huffman compression code

1 The first step is to read the data from the file and count which character appeared and with what frequency in the freq array.

Then we build a code tree, where we encode the most frequently appearing character with the minimum number of bits. 
Each character is represented by a set of 0 and 1 depending on the frequency.

Using union, we combine the bits of the code into a new character, consisting of 8 bits, in ASCII encoding

The last step is to write the compressed text back to the file
