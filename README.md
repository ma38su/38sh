# 38sh

38sh is a simple Shell. The shell supports only limited features.

The shell supports x86-64 Linux or macOS. It's tested on macOS Catalina.

## How to Build

Run make to build:

```
make
```

## How to run

```
./38sh
```

## Supported Features

### Built-in Command
- cd/chdir
- exit

### String Literal

```
$ grep "Hello World" *
$ grep 'Hello World' *
```

### Redirection and Pipe

```
$ command < File      # Input Redirection
$ command > File      # Output Redirection (Create & Truncate)
$ command >> File     # Output Redirection (Create & Append)
```

```
$ command1 | command2 # Pipe
```

Input/Output Redirection and Pipe can be used in combination.

```
$ command < File1 > File2
$ command1 < File1 | command2 > File2
```

### Multi Commands in one line

```
$ command1 && command2
$ command1; command2
```

## Author
ma38su

## Reference
1. ふつうのLinuxプログラミング 第2版
