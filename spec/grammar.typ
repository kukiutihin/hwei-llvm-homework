#set page(
  paper: "a4",
  margin: (top: 2.2cm, bottom: 2.2cm, left: 2.0cm, right: 2.0cm),
  header: context {
    if counter(page).get().first() > 1 [
      #set text(size: 8.5pt, fill: luma(110), style: "italic")
      Skib67 Language Reference — Formal Syntax
      #h(1fr)
      #counter(page).display()
      #v(-6pt)
      #line(length: 100%, stroke: 0.4pt + luma(180))
    ]
  },
  footer: context {
    if counter(page).get().first() == 1 [
      #align(center, text(size: 8.5pt, fill: luma(110), counter(page).display()))
    ]
  },
)

#set text(font: "New Computer Modern", size: 10pt, lang: "en")
#set par(justify: true, leading: 0.62em)

#set heading(numbering: "1.1")
#show heading.where(level: 1): it => {
  v(1.2em, weak: true)
  block(below: 0.6em)[
    #set text(size: 12pt, weight: "bold")
    #it
  ]
}
#show heading.where(level: 2): it => {
  v(0.9em, weak: true)
  block(below: 0.45em)[
    #set text(size: 10.5pt, weight: "bold")
    #it
  ]
}

#let nt(name) = emph(name)
#let kw(tok) = text(font: "New Computer Modern Mono", weight: "bold", size: 9pt, tok)
#let meta(sym) = text(fill: luma(90), sym)
#let opt(body) = [#meta("[") #body #meta("]")]
#let rep(body) = [#meta("{") #body #meta("}")]
#let grp(body) = [#meta("(") #body #meta(")")]
#let alt = [#h(0.35em) #meta("|") #h(0.35em)]
#let note(txt) = text(size: 8.5pt, style: "italic", fill: luma(100), [— #txt])

#let grammar(caption: none, ..rows) = {
  block(breakable: false, width: 100%, above: 0.7em, below: 0.9em)[
    #if caption != none [
      #text(size: 8.5pt, weight: "bold", fill: luma(70), upper(caption))
      #v(-4pt)
    ]
    #line(length: 100%, stroke: 0.75pt + black)
    #v(-4pt)
    #table(
      columns: (auto, auto, 1fr),
      stroke: none,
      inset: (x: 4pt, y: 3pt),
      align: (right, center, left),
      ..rows.pos().flatten()
    )
    #v(-6pt)
    #line(length: 100%, stroke: 0.75pt + black)
  ]
}

#let rule(lhs, rhs) = (nt(lhs), $::=$, rhs)
#let more(rhs) = ("", $|$, rhs)

#align(center)[
  #text(size: 17pt, weight: "bold")[The Skib67 Programming Language]
  #v(2pt)
  #text(size: 11pt, style: "italic", fill: luma(70))[Formal Lexical and Syntactic Specification]
]

#v(0.6em)
#line(length: 100%, stroke: 0.5pt + luma(160))
#v(0.3em)

#pad(x: 1.5em)[
  #text(size: 9pt)[
    *Abstract.* This document defines the formal context-free grammar of *Skib67*, a layout-insensitive, statically typed, expression-oriented language with uniform reference semantics where only one-dimensional arrays are mutable. Grammar productions are presented in Extended Backus–Naur Form (EBNF), where non-terminals appear in _italics_, terminal tokens in *`monospace`*, optional forms in `[`#sym.dots`]`, and zero-or-more repetitions in `{`#sym.dots`}`.
  ]
]

#v(0.3em)
#line(length: 100%, stroke: 0.5pt + luma(160))

#v(0.6em)
= Lexical Elements and Type Syntax
#v(0.3em)

Whitespace and line breaks act purely as token delimiters and carry no syntactic or positional significance. Value and function identifiers begin with a lowercase letter or underscore (optionally qualified by a `PascalCase` module prefix such as `Std.fmax`). Skib67 features a closed set of built-in types consisting solely of the unit type `()`, 64-bit scalar primitives (`Int`, `Uint`, `Float`, and `Bool`), and strictly one-dimensional `Array[T]` containers of scalar elements.

#grammar(
  rule("ident", [#grp[#nt("lower") #alt #kw("_")] #rep[#nt("letter") #alt #nt("digit") #alt #kw("_")]]),
  rule("mod-ident", [#nt("upper") #rep[#nt("letter") #alt #nt("digit")]]),
  rule("qual-ident", [#opt[#nt("mod-ident") #kw(".")] #nt("ident") #note("optional module prefix, e.g. Std.fmax")]),
  rule("literal", [#nt("hex-int") #alt #nt("float-lit") #alt #nt("int-lit") #alt #kw("true") #alt #kw("false")]),
  rule("hex-int", [#kw("0x") #nt("hex-digit") #rep[#nt("hex-digit")]]),
  rule("float-lit", [#nt("digit") #rep[#nt("digit")] #kw(".") #nt("digit") #rep[#nt("digit")]]),
  rule("int-lit", [#nt("digit") #rep[#nt("digit")]]),
  rule("type", [#kw("()") #alt #nt("scalar-type") #alt #nt("array-type")]),
  rule("scalar-type", [#kw("Int") #alt #kw("Uint") #alt #kw("Float") #alt #kw("Bool") #note("all scalars are 64-bit")]),
  rule("array-type", [#kw("Array") #kw("[") #nt("scalar-type") #kw("]") #note("one-dimensional only")]),
)

#v(0.6em)
= Top-Level Program and Declarations
#v(0.3em)

A Skib67 compilation unit consists of a sequence of top-level value and function declarations. Every function declaration—whether top-level or local—must specify full parameter type annotations and an explicit return type; nullary functions omit the parameter list.

#grammar(
  rule("program", [#rep[#nt("top-decl")]]),
  rule("top-decl", [#nt("func-decl") #alt #nt("val-decl")]),
  rule("func-decl", [#nt("ident") #opt[#nt("param-list")] #kw(":") #nt("type") #kw("=") #nt("expr")]),
  rule("val-decl", [#nt("ident") #opt[#kw(":") #nt("type")] #kw("=") #nt("value-expr")]),
  rule("param-list", [#kw("(") #opt[#nt("param") #rep[#kw(",") #nt("param")]] #kw(")")]),
  rule("param", [#nt("ident") #kw(":") #nt("type")]),
)

#v(0.6em)
= Expressions, Bindings, and Control Flow
#v(0.3em)

All constructs in Skib67 evaluate as expressions without any layout or indentation rules, and all values obey uniform reference semantics. Scalar values and variable bindings are strictly immutable: reference bindings (`x = e1 [e2]`) and local function declarations (`f (...) : T = body [cont]`) bind a single right-hand expression and automatically chain into an optional continuation expression without delimiters; if omitted at the end of a scope, they evaluate to `()`. Only one-dimensional arrays are mutable: in-place element mutations are restricted to array subscripts via the `*=` operator (`subscript *= e`) and evaluate to `()`. Like any other side-effecting expression, sequential array mutations must be explicitly chained with the sequencing operator `;`. Range expressions (`..` and `..=`) act as list-monad producers when bound and compile down to nested imperative `for` loops.

#grammar(
  rule("expr", [#nt("seq-expr")]),
  rule("seq-expr", [#nt("bind-expr") #rep[#kw(";") #nt("bind-expr")] #note("sequences side-effecting expressions")]),
  rule("bind-expr", [#nt("func-decl") #opt[#nt("expr")] #note("local function; optional next")]),
  more([#nt("ident") #opt[#kw(":") #nt("type")] #kw("=") #nt("value-expr") #opt[#nt("expr")] #note("immutable binding; optional next")]),
  more([#nt("mut-expr")]),
  more([#nt("value-expr")]),
  rule("mut-expr", [#nt("subscript") #kw("*=") #nt("value-expr") #note("in-place array mutation; returns ()")]),
  rule("value-expr", [#nt("if-expr") #alt #nt("bin-expr")]),
  rule("if-expr", [#kw("if") #nt("expr") #kw("then") #nt("expr") #kw("else") #nt("expr")]),
)

#v(0.6em)
= Operator Precedence and Primary Expressions
#v(0.3em)

Built-in binary and range operators are stratified by fixed syntactic precedence from lowest (`..`, `..=`) to highest (`*`, `/`, `%`), followed by prefix unary operators and primary expressions (function calls, single-dimension array subscripts, parenthesized expressions, literals, and identifiers). Nullary functions declared without a parameter list (e.g., `app : () = ...`) are invoked directly as bare identifiers without trailing parentheses.

#grammar(
  rule("bin-expr", [#nt("bit-or-expr") #opt[#grp[#kw("..") #alt #kw("..=")] #nt("bit-or-expr")] #note("range lowers to loop")]),
  rule("bit-or-expr", [#nt("bit-xor-expr") #rep[#kw("|") #nt("bit-xor-expr")]]),
  rule("bit-xor-expr", [#nt("bit-and-expr") #rep[#kw("^") #nt("bit-and-expr")]]),
  rule("bit-and-expr", [#nt("eq-expr") #rep[#kw("&") #nt("eq-expr")]]),
  rule("eq-expr", [#nt("rel-expr") #rep[#grp[#kw("==") #alt #kw("!=")] #nt("rel-expr")]]),
  rule("rel-expr", [#nt("shift-expr") #rep[#grp[#kw("<") #alt #kw("<=") #alt #kw(">") #alt #kw(">=")] #nt("shift-expr")]]),
  rule("shift-expr", [#nt("add-expr") #rep[#grp[#kw("<<") #alt #kw(">>")] #nt("add-expr")]]),
  rule("add-expr", [#nt("mul-expr") #rep[#grp[#kw("+") #alt #kw("-")] #nt("mul-expr")]]),
  rule("mul-expr", [#nt("unary-expr") #rep[#grp[#kw("*") #alt #kw("/") #alt #kw("%")] #nt("unary-expr")]]),
  rule("unary-expr", [#grp[#kw("-") #alt #kw("!") #alt #kw("~")] #nt("unary-expr") #alt #nt("primary-expr")]),
  rule("primary-expr", [#nt("call") #alt #nt("subscript") #alt #kw("(") #opt[#nt("expr")] #kw(")") #alt #nt("literal") #alt #nt("qual-ident")]),
  rule("call", [#nt("qual-ident") #kw("(") #opt[#nt("arg-list")] #kw(")") #note("elided for 0-arg functions")]),
  rule("arg-list", [#nt("expr") #rep[#kw(",") #nt("expr")]]),
  rule("subscript", [#nt("qual-ident") #kw("[") #nt("expr") #kw("]") #note("single-dimension indexing")]),
)
