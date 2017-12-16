int j;
float g(int i, bool b) {
  if i < j && b {
    return i;
  } else {
    return 3.14;
  }
}
j = 1;
print(call g(j * 0.5, 2 == 3));