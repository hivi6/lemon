# lemon

An incrementally made compiler

## Building

Building can be done by simplying running a basic make command:

```bash
make
```

This should create the "compiler" in `./build` directory

## Testing

You can run the test programs like the fibonacci:

```bash
./build/lemon tests/fib.lemon
```

You can also run code from command line:

```bash
echo "var a = 10; print a + 2;" | ./build/lemon -
```

## Usage

You can get the usage of the program by running the following:

```bash
./build/lemon --help
```

This should print the following screen:

```bash
USAGE: ./lemon [flags] <filename>

FLAGS:
    --help, -h       This screen
    --only-tokens    Only print tokens
    --only-ast       Only print ast
    --only-st        Only print symbol table
    --only-ir        Only print ir
    --only-vm-state  Only print vm state

MORE INFO:
    -> To read from stdin run as follows './lemon -'
```

## Resources

- [Grammar for lemon](./grammar)

