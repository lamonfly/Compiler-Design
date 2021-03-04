## Compiler Design
This compiler reads from a input text file using the console and output code in the moon language (RISC). It was made for a compiler design class (COMP442).

We first read the file in order to tokenize every keyword of a set lingo. We use the dfa shown below to classify every token.
![alt dfa](https://github.com/lamonfly/Compiler-Design/blob/master/dfa.png?raw=true)

We then make sense of the array of token into grammar using a grammar text file.

Then we create tables for classes, functions, variables and etc.

Now that all the information from the input text file code has been processed.

We transfer the data into a risc architecture, moon code to be tested.
