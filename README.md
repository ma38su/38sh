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

### Supported Built-in Command
- cd/chdir
- exit

### Supported String Literal

```
$ cat Hello World | grep "Hello World"
Hello World
$ cat Hello World | grep 'Hello World'
Hello World
```

### Supported Redirection and Pipe

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
$ command < file1 > file2      # file1 to STDIN, STDOUT to file2
$ command1 < file1 | commmand2 | command3 > file2 # file1 to STDIN, STDOUT to file2
```

## Author
ma38su

## Reference
1. ふつうのLinuxプログラミング 第2版
