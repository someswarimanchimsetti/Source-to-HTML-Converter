# Source-to-HTML Converter

## Overview
A C-based application that converts C source code into an HTML file with syntax highlighting. The generated HTML page makes source code easier to read in a web browser.

## Features
- Highlights C keywords and identifiers.
- Highlights preprocessor directives and header files.
- Highlights strings and character literals.
- Highlights single-line and multi-line comments.
- Highlights numeric constants.
- Escapes HTML special characters such as `<`, `>` and `&`.
- Uses a modular design with separate source and header files.
- Includes a Makefile for building and cleaning the project.

## Project Structure
- `main.c` — Main program and token-based HTML generation.
- `html.c` — HTML output and helper functions.
- `html.h` — HTML-related function declarations.
- `lexer.c` — Tokenization of C source code.
- `lexer.h` — Lexer structures, token types, and function declarations.
- `sample.c` — Sample C source file for testing.
- `Makefile` — Build instructions and cleanup.
- `README.md` — Project documentation.

## Requirements
- GCC compiler
- GNU Make
- A web browser

## Build
```bash
make
```

## Run
```bash
./source2html sample.c
```

The program generates `output.html`. Open this file in a web browser to view the syntax-highlighted source code.

## Clean
```bash
make clean
```

## Technologies Used
- C programming
- File handling
- String processing
- Lexical analysis
- HTML and CSS
- GCC and GNU Make

## Author
Someswari Manchimsetti
