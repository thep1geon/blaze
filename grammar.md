program          = { func | class };

class            = "class", ident, "{", { ["@"], func }, "}";

func             = "func", ident, "(", { ident, [","] }, ")", stmt;

stmt             = "{", stmt, "}" |
                  "if", expr, stmt |
                  "while", expr, stmt |
                  "for", ident, ":", expr, stmt |
                  "return", expr, ";" |
                  "let", { ident, [","] }, ["=", expr], ";" |
                  expr, ";" |
                  ";";

expr             = str_lit | int_lit | ident |
                  "(", expr, ")" |
                  expr, "[", expr, "]" |
                  expr, "(", { expr, [","] }, ")" |
                  "[", { expr, [","] }, "]" |
                  ident, "::", expr |
                  expr, ("+" | "-" | "*" | "/" | "+=" | "-=" | "*=" | "/=" | "==" | "not" | "or" | "and" | ","),
                  expr |
                  ("--" | "++" | "!" | "-"), expr |
                  expr, ("++" | "--");

str_lit          = '"', { "char" | "digit" }, '"';

int_lit          = "digit", { "digit" | "." };

ident            = "char", { "digit" | "char" | "_" };
