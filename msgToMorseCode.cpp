//============================================================================
// Name        : Image Processing using C++
// Author      : Piyush Khopkar
// Description : Convert input text into Morse Code
//============================================================================


#include <iostream>
#include <string>
#include <list>
#include "stdio.h"
using namespace std;

/* Message class */
class Message{
public:
	string s_Message;

	/* Constructors.*/
	Message(){
		cout << "Enter input string" << endl;
		cin >> s_Message;
	}
	Message(string inp_Message){
		cout <<"Calling parametric constructor"<<endl;
		s_Message = inp_Message;
	}
	/* Destructor.*/
	~Message(){
		delete [] &s_Message;
	}

	/* Member functions.*/
	void printMessage(){
		cout << "The input message is: " << s_Message <<endl;
	}
};

/* MorseCodeMessage class */
class  MorseCodeMessage : public Message
{
	public:
	string inputString, outString;
	/*Constructor*/
	MorseCodeMessage():Message()
	{
		inputString = s_Message;
		cout << "You Entered = "<<inputString<<endl;
	}
	MorseCodeMessage(string str):Message(str)
	{
		inputString = str;
		cout << "Default String to convert is =  "<<inputString<<endl;
	}
	/* Methods*/
	void translateMessage2Morse()
	{
		char morseCode[29][7]={".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", ".--.-" , ".-.-", "---."};
		char dictionary [26][1]={ 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
		int j, k;
		int option=0;
		unsigned int i;
		int find_string = 0;
		int stop_string = 29;
		unsigned int str_length;
		str_length=inputString.size();
		for (i=0;i<str_length;i++)
		{
			find_string=0;
			while (find_string < stop_string)
			{
				if (inputString[i]==dictionary[find_string][0])
				{
					int j = 0;
					while(morseCode[find_string][j] != NULL)
					{
						outString.push_back(morseCode[find_string][j]);
						j++;
					}
					break;
				}
				else
					find_string = find_string+1;
			}
		}
		cout<<"Conversion complete"<<endl;
	 }
	 void printMessage()
	 {
	 	cout << "The input message is: " << inputString <<endl;
	 	cout << "The converted message is: " << outString<<endl;
	 }
};

/* Message class */
class MessageStack{
public:
	string Stack[6];
	string printThis, typeOfOutput;
	int n;

	/* Constructor */
	MessageStack()
	{
		n = 0;
	}
	MessageStack(MorseCodeMessage msg)
	{
		PushData(msg.inputString);
		n++;
		PushData(msg.outString);
		n++;
	}
	/* Destructor */
	~MessageStack(){
		string Stack = NULL;
	}

	/* Push data*/
	void PushData(string inp_data)
	{
		int j = 0;
		while(inp_data[j] != NULL)
		{
			Stack[n].push_back(inp_data[j]);
			j++;
		}
		n++;
	}

	/* Pop data */
	string PopData()
	{
		string out_data = Stack[n-1];
		n--;
		return(out_data);
	}
	void printStack(string printThis,string typeOfOutput)
	{
		cout<<typeOfOutput<<printThis<<endl;
	}
};

/* Main function */
int main(){

	MessageStack stack;

	MorseCodeMessage morse;
	morse.translateMessage2Morse();
	stack.PushData(morse.inputString);
	stack.PushData(morse.outString);

	cout << "End"<<endl;

	for (int i=0; i<1;i++)
	{
		stack.printStack(stack.PopData(),"Morse Code = ");
		stack.printStack(stack.PopData(),"Equivalent Original Message = ");

	}
	return 0;
}

/* end of file */