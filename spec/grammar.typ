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

// Section numbering and academic heading styling
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

// Non-terminal (italic serif)
#let nt(name) = emph(name)

// Terminal keyword or symbol (bold monospace)
#let kw(tok) = text(font: "New Computer Modern Mono", weight: "bold", size: 9pt, tok)

// Meta-syntax symbols (light gray/muted)
#let meta(sym) = text(fill: luma(90), sym)
#let opt(body) = [#meta("[") #body #meta("]")]
#let rep(body) = [#meta("{") #body #meta("}")]
#let grp(body) = [#meta("(") #body #meta(")")]
#let alt = [#h(0.35em) #meta("|") #h(0.35em)]

// Inline comment inside grammar block
#let note(txt) = text(size: 8.5pt, style: "italic", fill: luma(100), [— #txt])

// Production table container with top/bottom academic booktabs rules
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

// Helper for a single production rule (first alternative)
#let rule(lhs, rhs) = (nt(lhs), $::=$, rhs)
// Helper for continuation alternatives
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
    *Abstract.* This document defines the formal context-free grammar of *Skib67*, a layout-insensitive, statically typed, expression-oriented language. Grammar productions are presented in Extended Backus–Naur Form (EBNF), where non-terminals appear in _italics_, terminal tokens in *`monospace`*, optional forms in `[`#sym.dots`]`, and zero-or-more repetitions in `{`#sym.dots`}`.
  ]
]

#v(0.3em)
#line(length: 100%, stroke: 0.5pt + luma(160))

#v(0.6em)
= Lexical Elements and Type Syntax
#v(0.3em)

Whitespace and line breaks act purely as token delimiters and carry no syntactic or positional significance. Identifiers follow casing conventions to distinguish value bindings from module and type names: value and function identifiers begin with a lowercase letter or underscore (optionally qualified by a module prefix such as `Std.fmax`), whereas modules and type constructors use `PascalCase`. Skib67 provides built-in scalar types (`Int`, `Uint`, `Float`, `Double`, `Bool`), the unit type `()`, and parameterized `Array[T]` containers.

#grammar(
  rule("ident", [#grp[#nt("lower") #alt #kw("_")] #rep[#nt("letter") #alt #nt("digit") #alt #kw("_")]]),
  rule("qual-ident", [#opt[#nt("type-ident") #kw(".")] #nt("ident") #note("optional module prefix, e.g. Std.fmax")]),
  rule("type-ident", [#nt("upper") #rep[#nt("letter") #alt #nt("digit")]]),
  rule("literal", [#nt("hex-int") #alt #nt("float-lit") #alt #nt("int-lit") #alt #kw("true") #alt #kw("false")]),
  rule("hex-int", [#kw("0x") #nt("hex-digit") #rep[#nt("hex-digit")]]),
  rule("float-lit", [#nt("digit") #rep[#nt("digit")] #kw(".") #nt("digit") #rep[#nt("digit")]]),
  rule("int-lit", [#nt("digit") #rep[#nt("digit")]]),
  rule("type", [#kw("()") #alt #nt("builtin-type") #alt #nt("type-ident") #opt[#kw("[") #nt("type-args") #kw("]")]]),
  rule("builtin-type", [#kw("Int") #alt #kw("Uint") #alt #kw("Float") #alt #kw("Double") #alt #kw("Bool")]),
  more([#kw("Array") #kw("[") #nt("type") #kw("]")]),
  rule("type-args", [#nt("type") #rep[#kw(",") #nt("type")]]),
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

All constructs in Skib67 evaluate as expressions without any layout or indentation rules. Immutable variable bindings (`x = e1 e2`) and local function declarations (`f (...) : T = e1 e2`) bind a single right-hand expression and automatically chain into their continuation expression `e2` without delimiters. In contrast, all in-place mutations use the `*=` operator (`place *= e`) and evaluate to `()`; like any other side-effecting expression, sequential mutations must be explicitly chained with the infix `;` operator. Range expressions (`..` and `..=`) act as list-monad producers when bound and compile down to nested imperative `for` loops.

#grammar(
  rule("expr", [#nt("seq-expr")]),
  rule("bind-expr", [#nt("func-decl") #nt("expr") #note("local function chains next")]),
  more([#nt("ident") #opt[#kw(":") #nt("type")] #kw("=") #nt("value-expr") #nt("expr") #note("immutable binding chains next")]),
  more([#nt("mut-expr")]),
  more([#nt("value-expr")]),
  rule("mut-expr", [#nt("place") #kw("*=") #nt("value-expr") #note("in-place mutation; returns ()")]),
  rule("place", [#nt("ident") #rep[#nt("index-suffix")]]),
  rule("value-expr", [#nt("if-expr") #alt #nt("range-expr")]),
  rule("if-expr", [#kw("if") #nt("expr") #kw("then") #nt("expr") #kw("else") #nt("expr")]),
)

#v(0.6em)
= Operator Precedence and Postfix Application
#v(0.3em)

All operators—including the side-effect sequencing operator `;`—are infix functions ordered from lowest precedence (`;`) to highest precedence (`*`, `/`, `%`), followed by prefix unary operators and left-associative postfix application (function calls and array indexing). Nullary functions declared without a parameter list (e.g., `app : () = ...`) are invoked directly as bare identifiers without trailing parentheses.

#grammar(
  rule("seq-expr", [#nt("bind-expr") #rep[#kw(";") #nt("bind-expr")] #note("lowest-precedence infix operator")]),
  rule("range-expr", [#nt("bit-or-expr") #opt[#grp[#kw("..") #alt #kw("..=")] #nt("bit-or-expr")] #note("lowers to loop")]),
  rule("bit-or-expr", [#nt("bit-xor-expr") #rep[#kw("|") #nt("bit-xor-expr")]]),
  rule("bit-xor-expr", [#nt("bit-and-expr") #rep[#kw("^") #nt("bit-and-expr")]]),
  rule("bit-and-expr", [#nt("eq-expr") #rep[#kw("&") #nt("eq-expr")]]),
  rule("eq-expr", [#nt("rel-expr") #rep[#grp[#kw("==") #alt #kw("!=")] #nt("rel-expr")]]),
  rule("rel-expr", [#nt("shift-expr") #rep[#grp[#kw("<") #alt #kw("<=") #alt #kw(">") #alt #kw(">=")] #nt("shift-expr")]]),
  rule("shift-expr", [#nt("add-expr") #rep[#grp[#kw("<<") #alt #kw(">>")] #nt("add-expr")]]),
  rule("add-expr", [#nt("mul-expr") #rep[#grp[#kw("+") #alt #kw("-")] #nt("mul-expr")]]),
  rule("mul-expr", [#nt("unary-expr") #rep[#grp[#kw("*") #alt #kw("/") #alt #kw("%")] #nt("unary-expr")]]),
  rule("unary-expr", [#grp[#kw("-") #alt #kw("!") #alt #kw("~")] #nt("unary-expr") #alt #nt("postfix-expr")]),
  rule("postfix-expr", [#nt("primary-expr") #rep[#nt("call-suffix") #alt #nt("index-suffix")]]),
  rule("call-suffix", [#kw("(") #opt[#nt("arg-list")] #kw(")") #note("elided for 0-arg functions")]),
  rule("arg-list", [#nt("expr") #rep[#kw(",") #nt("expr")]]),
  rule("index-suffix", [#kw("[") #nt("expr") #kw("]")]),
  rule("primary-expr", [#kw("(") #opt[#nt("expr")] #kw(")") #alt #nt("literal") #alt #nt("qual-ident")]),
)
