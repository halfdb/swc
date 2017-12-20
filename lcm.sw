int a;
int b;

int _gcd(int a, int b) {
  int c;
  c = a % b;
  if c == 0 {
    return b;
  } else {
    return call _gcd(b, c);
  }
}

int gcd(int a, int b) {
  if a < b {
    return call _gcd(b, a);
  } else {
    return call _gcd(a, b);
  }
}

print("a=?");
read(a);
print("b=?");
read(b);
print("lcm is ", a * b / call gcd(a, b));

