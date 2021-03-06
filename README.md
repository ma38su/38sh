# 38sh

38sh is a simple Shell. The shell supports only limited features.

The shell supports x86-64 Linux or macOS. It's tested on macOS Catalina.

## How to Build

Run make to build:

```
make
```

Run make to build and test:

```
make test
```

## How to run

```
./38sh
```

or 

```
./38sh input.sh
```

## Supported Features

### Built-in Commands

- cd/chdir
- exit

### Environment Variables

Environment variables is supported to read only.

```
$ echo $HOME
/home/ma38su
$ echo ${HOME}
/home/ma38su
```

### String Literal

```
$ echo "$HOME"
/home/ma38su
$ echo '$HOME'
$HOME
```

### Pipe

```
$ command1 | command2
$ command1 | command2 | command3
```

### Redirection

```
$ command < File      # STDIN Redirection
$ command > File      # STDOUT Redirection (Create & Truncate)
$ command >> File     # STDOUT Redirection (Create & Append)

$ command 2> File     # STDERR Redirection (Create & Truncate)
$ command 2>> File    # STDERR Redirection (Create & Append)

$ command &> File     # STDOUT & STDERR Redirection (Create & Truncate)
$ command &>> File    # STDOUT & STDERR Redirection (Create & Append)
```

Input/Output Redirection and Pipe can be used in combination.

```
$ command < File1 > File2
$ command1 < File1 | command2 > File2
```

### Multi Commands in one line

```
$ echo hello && world
hello
world
$ echo hello; echo world
hello
world
```

## Author
ma38su

## Reference
1. 青木 峰郎, ふつうのLinuxプログラミング 第2版, SBクリエイティブ (2017)

