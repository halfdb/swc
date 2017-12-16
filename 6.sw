int n;
int f(int n) {
  if n==0 {
    return 0;
  } else {
    return call f(n-1) + 1;
  }
}
print("input the n");
read(n);
print (call f(n));