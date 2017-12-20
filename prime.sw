int i;
bool isPrime(int n) {
  int i;
  for i in 2...n {
    if n % i == 0 {
      return false;
    }
  }
  return true;
}

for i in 2...101 {
  if call isPrime(i) {
    print(i, " is prime");
  }
}

