#include <iostream>

using namespace std;

class A 
{
    public:
        A() { cout << "A()" << endl; x = 0; c = -666; };
        
		A(int x) : A() { cout << "A(x)" << endl;  this->x = x; };
        
		~A() { cout << "~A()" << endl; };
        
    int x, c;
};

class B : public A 
{
    public:
        
		B(int d, int x) : A(x)  { cout << "B(d,x)" << endl; this->d = d; };
        
		~B() { cout << "~B()" << endl; };
        
    int d;
};



int main()
{
   // A *a = new A(1);
    
    B *b = new B(2, 3); // A() , then A(x) then B(d,x) gets called 
    
   // cout << a->x << " , " << a->c << endl;
    cout << b->x << " , " << b->c << " , " << b->d << endl;

	/*
	
	output: 
	
	A()
	A(x)
	B(d,x)
	
	3 , -666 , 2
	
	
	*/


    return 0;
}