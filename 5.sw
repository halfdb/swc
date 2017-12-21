int n;
int f(int n) {
  int a;
  int b;
  int c;
  int i;
  if n == 0 || n == 1 {
    return n;
  }

  a = 0; b = 1;
  for i in 2...n+1 {
    c = b;
    b = a + b;
    a = c;
  }
  return b;
}
for n in 0...11 {
print("f ", n, " is ", call f(n));
}
n=1;
while (n != 0) {
print("f ", n, " is ", call f(n));
print("input the n");
read(n);
}
