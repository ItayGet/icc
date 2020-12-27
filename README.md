# ICC - Itay's C Compiler
This project's mission is to build a self-hosting C compiler that will support most of C90's features.
The backend will generate x86 assembly.


## Features
### Lexer:
- [x] Identifiers
- [x] String literals
- [x] Integer constants
- [ ] Floating constants
- [x] Punctuators
- [ ] Preprocessor tokens
- [ ] Integer constant postfixes
### Parsing and intermediate code generation
- [ ] A type system. Current type system is not according to the C standard
- [x] A symbol table
- [x] Expressions for addition, subtraction and assignment
- [x] Implicit casts
- [ ] Errors
- [ ] Constants
- [ ] Any kind of statement
### Back-end
- [ ] Nothing
