#include <iostream>        

using namespace std;        
int counter = 0;
#define sens_pin 14    // sensor pin D0
int main()
{
pinMode(sens_pin, INPUT);        // Configure GPIO1 as an input

cout<<"Counter started"<<endl;
cout<<"Number of coins: "<< counter << endl;


// Main program loop
while(1)
{
    // check if digitalRead returns 0, means coin detected
    if(digitalRead(sens_pin) == 0)
{
    counter++;
    delay(500);
    cout<<"Number of coins: "<< counter << endl;
}
}
    return 0;
}