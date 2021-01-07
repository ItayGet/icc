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
- [x] A symbol table
- [x] Expressions for addition, subtraction and assignment
- [ ] Expression with short-circuit code
- [ ] Errors
- [ ] Constants
- [ ] Any kind of statement
### Type system
- [x] Basic type system
- [x] Implicit casts
- [ ] Functions
- [ ] Getting sizes of basic types or record types
- [ ] Getting the right type for expression like pointer subtraction or the sizeof operator
### Back-end
- [ ] Nothing
