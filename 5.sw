int n;
int f(int n) {
  if n == 0 || n == 1 {
    return 1;
  } else {
    return call f(n-1) + call f(n-2);
  }
}

for n in 0...11 {
print("f ", n, " is ", call f(n));
}