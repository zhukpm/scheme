# Basic implementation of Scheme programming language Interpreter

## Examples
    > (+ 1 8)
    9
    > (* 8 (+ 1 9))
    80
    > (define len (lambda x (if (null? x) 0 (+ 1 (len (cdr x))))))
    ()
    > (len '(5 9 7 1 '(a b) (+ 8 9)))
    6
    > (define fib (lambda n (if (< n 3) 1 (+ (fib (- n 1)) (fib (- n 2))))))
    ()
    > (fib 8)
    21

## Supported data types

* Number
* Boolean
* Symbol - a name to lookup in Scope
* Pair (List) - expression to evaluate or store data

Numbers and Booleans are self-evaluating.

All objects evaluate to TRUE except for `#f` when used in predicates places 

## Supported functions, operators and predicates
Each function has a name, a number of arguments and
a returning object - the result of function execution

Syntax: `(function-name argument1 argument2 ...)` -> result (object)

### General functions

| Name | # arguments | arg sequence | logic | returns |
| --- | --- | --- | --- | --- |
| quote  | 1 | | returns argument itself, without evaluation. If expression is a Cell - it becomes a List of items | object |
| max  | 1+ | all numbers | returns max argument number | number |
| min  | 1+ | all numbers | returns min argument number | number |
| abs  | 1 | a number | returns abs of argument | number |
| lambda  | 2+ | symbol / list of symbols, expressions | creates new function that takes 0 or more arguments (as defined by the first argument) and executes a list of expressions (arguments 2...). Returns the resulting object of the last expression when applied | function object |

### Scope-updating functions

| Name | # arguments | arg sequence | logic | returns |
| --- | --- | --- | --- | --- |
| define  | 2 | symbol name / Pair (for defining lambdas) , object value | creates variable with "name" associated with "value" in current Scope | object, empty list () (null) |
| set!  | 2 | symbol name, object value | updates variable with "name": sets "value" in current Scope. If no "name" in current Scope defined - throws an Error | object, empty list () (null) |

### Operators

| Name | # arguments | arg sequence | logic | returns |
| --- | --- | --- | --- | --- |
| + | 0+ |  |  sums all !numbers!, starting from 0 | a number - result | 
| - | 1+ |  |  subtracts all numbers from the 1st one. If only one number is given - negates it. | a number - result |
| * | 0+ |  |  product of all !numbers!, starting from 1 | a number - result |
| / | 1+ |  |  successive division of numbers one by one. Or 1/num if only one is given | a number - result |
| if | 2, 3 | 1st - expr that evaluates to get boolean value. 2nd (+3rd) - expr to return  | if 1st arg evals to true - eval and return 2nd argument; if 1st arg evals to false - eval and return 3rd argument, if no 3rd arg - return ()  | evaluation of returning expression or () if nothing to return |

Boolean operators

| Name | # arguments | arg sequence | logic | returns |
| --- | --- | --- | --- | --- |
| and  | 0+ | | (and \<test\>) The \<test\> expressions are evaluated from left to right, and the value of the first expression that evaluates to a false value is returned. Any remaining expressions are not evaluated. If all the expressions evaluate to true values, the value of the last expression is returned. If there are no expressions then #t is returned. | boolean |
| or  | 0+ | | (or \<test>) The \<test> expressions are evaluated from left to right, and the value of the first expression that evaluates to a true value is returned. Any remaining expressions are not evaluated. If all expressions evaluate to false values, the value of the last expression is returned. If there are no expressions then #f is returned. | boolean |
| not  | 1 | | negates boolean representation of the argument | boolean |
| < | 0, 2+ |  |  compares for < pairs of numbers one by one (1-2, 2-3, ...) | boolean |
| <= | 0, 2+ |  |  compares for <= pairs of numbers one by one (1-2, 2-3, ...) | boolean |
| \> | 0, 2+ |  |  compares for > pairs of numbers one by one (1-2, 2-3, ...) | boolean |
| \>= | 0, 2+ |  |  compares for >= pairs of numbers one by one (1-2, 2-3, ...) | boolean |
| = | 0, 2+ |  |  compares for equality all !numbers! | boolean |

### Predicates

| Name | # arguments | arg sequence | logic | returns |
| --- | --- | --- | --- | --- |
| boolean? | 1 | | Checks if arg has boolean type | boolean #t / #f |
| number? | 1 | | Checks if arg has number type | boolean #t / #f |
| symbol? | 1 | | Checks if arg has symbol type | boolean #t / #f |
| pair? | 1 | | Checks if arg has pair type | boolean #t / #f |
| list? | 1 | | Checks if arg has list type | boolean #t / #f |
| null? | 1 | | Checks if arg is an empty list - () | boolean #t / #f |

### Pair and List functions

| Name | # arguments | arg sequence | logic | returns |
| --- | --- | --- | --- | --- |
| cons  | 2 | | creates a pair; sets its left part to the 1st argument, and right part to the 2nd argument | pair |
| car | 1 | pair | extracts and returns the left part of the given pair (head of the given list) | object |
| cdr | 1 | pair | extracts and returns the right part of the given pair (tail of the given list) | object |
| set-car!  | 2 | pair object, object value | updates pair, sets its left part to object value | object, empty list () (null) |
| set-cdr!  | 2 | pair object, object value | updates pair, sets its right part to object value | object, empty list () (null) |
| list  | 0+ | | creates a new list from the given arguments | list |
| list-ref  | 2 | pair object, numeric index | extracts object at position `index` of the given list | object |
| list-tail | 2 | pair object, numeric index | extracts list tail after position `index` of the given list | list |
