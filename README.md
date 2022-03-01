# Huffman-Encoder-and-Decoder
Encoder and Decoder using Huffman coding that converts string to bit code and vice versa.

Input/Output Specifications:
The executable "huffman" is going to take four command line arguments depending on its mode of encode or decode.

Encode: /> ./huffman encode plainText.txt codeTable.txt encodedText.txt

where encode indicates that the program will be in encoding mode;
plainText.txt will be the path of the file that contains the text that you will encode;
codeTable.txt will be the path of the file that you will write the code table to;
encodedText.txt will be the path of the file that you will write the encoded text to;

Decode: /> ./huffman decode codeTable.txt encodedText.txt decodedText.txt

where decode indicates that the program will be in decoding mode;
codeTable.txt will be the path of the file that you will read the code table from;
encodedText.txt will be the path of the file that you will read the encoded data from;
decodedText.txt will be the path that you will write the decoded text to;
