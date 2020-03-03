# 38sh

38sh is a simple Shell. The shell supports only limited features.

The shell supports x86-64 Linux or macOS. It's tested on macOS Catalina.

## How to Build

Run make to build:

```
make
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

### Supported Pipe

```
$ command1 | command2 # command1 STDOUT to command2 STDIN
```

### Supported Redirect

```
$ command > file      # STDOUT to File (Create & Truncate)
$ command >> file     # Stdout to File (Create & Append)
```

## Author
ma38su

## Reference
1. ふつうのLinuxプログラミング 第2版
