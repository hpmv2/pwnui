## Pwnui's Approach to IO

### Structured IO

Every piece of data read and written via the pwnui IO script is structured and understood by the tool. Of course, the user may choose to use the raw API all the time, but using structured IO allows better automation, visualization and error detection.

### Read Syntax

The global variable, "s", is provided to the script, which represents the process. The stream operator ">>" is used to read data from the process via stdout.

Each element provided to the right hand side of ">>" is a *consumer*. Each consumer defines:

* The criteria of what data it accepts;
* Any interpreted information extracted from the data it consumes;
* What to do in the case of unexpected input or timeout.

Consumers may be chained and nested; we'll look at a few examples first to build some intuition:

```
# Reads a single line, parses it as a decimal integer, and assign the integer to x
x = s >> line.int

# Consume stdout until encountering the string "> "
s >> any >> "> "

# Read lines until encountering a line that ends with an integer; assign that
# integer to x and the final line's content to y
x, y = s >> any >> line(any >> regex("[0-9]+\\n").int).raw

# Read until seeing "abc", but don't consume the "abc" itself, and return the text read.
x = s >> any.raw >> "abc".peek

# Read either "Invalid choice", or until "> ", preferring "Invalid choice" if we see it.
invalid = s >> oneof(lit("Invalid choice").seen, any >> "> ")
```

This syntax is arguably verbose, and certainly more verbose than what pwnlib provides. However, it provides significantly greater flexibility, and also allows for lazy IO. Lazy IO is an opt-in mechanism where data is only read when absolutely necessary; for example, the second example, `s >> any > "> "`, does not block until later some reading operation extracts at least one value. This is sometimes helpful to speed up remote execution of exploits.

Here are some of the basic consumers:

* `lit("abc")` or just `"abc"`: Consumes the literal string. Pair with `any` to achieve "contains", "starts with", "ends with" functionality.
* `line`: Consumes a complete line. The line must end with a newline character and start either right after a newline character or at the first character of the read operation.
* `regex("...")`: Consumes data that matches the regex (completely, not partially).
* `oneof(c1, c2, ...)`: Attempts to consume with consumer `c1`, failing that, rolls back and attempts with `c2`, etc, until one succeeds.
* `nc(4)`: Consumes 4 (or some other number of) characters.
* `any`: A special consumer which will consume zero or more characters to help surrounding consumers succeed.

Consumers may be chained with `>>`, to become a single consumer. For example, `s >> (line >> line).raw` reads two lines, returning the combined content of those two lines. Special rules apply when chaining with `any`; for example, `any >> any` is not allowed.

Consumers may have further constraining predicates. These are specified by using the call syntax on the consumer, like `line("abc" >> any)`. The argument passed to the call is either a consumer, in which case the content must be completely consumable by that consumer, or a one-parameter boolean-returning lambda, which acts as a predicate of whether the data consumed by the consumer is acceptable.

Consumers may be suffixed with `.peek`, which means that it will attempt to consume, but before declaring success, it will put back the consumed content.

Consumers may be suffixed with "interpreters" which causes a return value to be generated, according to the parsing rules of the interpreter. Common interpreters include:

* `raw`: Return the consumed data as is (as a string).
* `int`: Return the consumed data as a decimal stringified integer, like `"100"` becomes `100`.
* `pint`: Return the consumed data interpreted as a packed unsigned integer encoded in little-endian.
* `seen`: Return a boolean if this consumer succeeded during this operation (useful when using a `oneof`).

Finally consumers may be suffixed with `.timeout(secs)` which causes the consumer to fail if this amount of time has passed before the consumer could succeed. TODO: Add more details here.
