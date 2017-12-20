int i;
bool f(int n){
  int i;int j;
  int k;
  for i in n-100...n
  {
    k=i;
    j=i;
    while(j>0)
    {
      k=k+j%10;
      j=j/10;
    }
    if(k==n){ return false; }
  }
  return true;
}
for i in 1...1000 {
  if call f(i){
    print(i);
  }
}
