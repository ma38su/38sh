#!/bin/bash

try() {
  expected="$1"
  input="$2"

  echo "$input" > tmp.sh
  actual=`./38sh tmp.sh`

  if [ "$actual" = "$expected" ]; then
    echo "\"$input\" => $actual is expected"
    echo ""
  else
    echo "\"$input\" => $actual is not expected. $expected is expected,"
    echo ""
    exit 1
  fi
}

try 'Hello' 'echo Hello'
try 'Hello
Hello' 'echo Hello; echo Hello'

try 'Hello
Hello' 'echo Hello && echo Hello'

try 'World' 'echo World | cat'
try '38sh' 'echo 38sh | cat > tmp.txt; cat tmp.txt'

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

echo "All tests are passed."
