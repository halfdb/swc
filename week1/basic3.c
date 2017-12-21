#include <stdio.h>
int x;

int y;
void abs()
{
  int i;
  
   int j;

   int a;
   for( i= 1;i<x;i++){
    
     for (j= 1;j<i;j++){

       a=i - j;      
       printf("%d\n",a);
    
     }; 
  
   };

} 

int main() {
scanf("%d",&x);

abs();
return 0;
}
