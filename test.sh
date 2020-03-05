#!/bin/bash

if [ ! -d tmp ]; then
  mkdir tmp
fi

try() {
  expected="$1"
  input="$2"

  echo "$input" > tmp/tmp.sh
  actual=`./38sh tmp/tmp.sh`

  if [ "$actual" = "$expected" ]; then
    echo "\"$input\" => \"$actual\" is expected"
    echo ""
  else
    echo "\"$input\" => \"$actual\" is not expected. \"$expected\" is expected,"
    echo ""
    exit 1
  fi
}

TMP_OUT=tmp/out.txt
TMP_ERR=tmp/err.txt

try 'Hello' 'echo Hello'
try 'Hello
Hello' 'echo Hello; echo Hello'

try 'Hello
Hello' 'echo Hello && echo Hello'

try 'World' 'echo World | cat'

try '38sh' "echo 38sh | cat > $TMP_OUT; cat $TMP_OUT"
try 'abc
def' "echo abc | cat > $TMP_OUT; echo def >> $TMP_OUT; cat $TMP_OUT"

try '' "./test/caterr err > $TMP_OUT; cat $TMP_OUT"

try 'abc' "./test/caterr abc 2> $TMP_ERR; cat $TMP_ERR"
try '38err' "./test/caterr 38err 2> $TMP_ERR; cat $TMP_ERR"
try '38err
38err' "./test/caterr 38err 2> $TMP_ERR; ./test/caterr 38err 2>> $TMP_ERR; cat $TMP_ERR"

try 'shell' "echo shell &> $TMP_OUT; cat $TMP_OUT"
try 'shell
shell2' "echo shell2 &>> $TMP_OUT; cat $TMP_OUT"
try 'shell' "./test/caterr shell &> $TMP_OUT; cat $TMP_OUT"
try 'shell
shell2' "./test/caterr shell2 &>> $TMP_OUT; cat $TMP_OUT"

try $HOME 'echo $HOME'
try $HOME 'echo "$HOME"'
try $HOME 'echo ${HOME}'
try '$HOME' 'echo '"'"'$HOME'"'"
try '' echo ${MA38SU}
try "$HOME$PATH" 'echo "$HOME$PATH"'
try "$HOME$PATH" 'echo "$HOME${PATH}"'
try "$HOME$PATH" 'echo "${HOME}$PATH"'
try "$HOME$PATH" 'echo "${HOME}${PATH}"'
try "$HOME $PATH" 'echo "$HOME $PATH"'
try "$HOME $PATH" 'echo "$HOME ${PATH}"'
try "$HOME $PATH" 'echo "${HOME} $PATH"'
try "$HOME $PATH" 'echo "${HOME} ${PATH}"'

rm $TMP_OUT $TMP_ERR

echo "All tests are passed."
