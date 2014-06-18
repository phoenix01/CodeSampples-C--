//============================================================================
// Name        : Morse Code to ARM board board
// Author      : Piyush Khopkar
// Version     :
// Description : This code will convert the corresponding morse code to lights and     sound. This code must be run with TS7250 ARM board
//============================================================================

#include <iostream>
#include <string>
#include <list>
#include "stdio.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

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
//	~Message(){
//		delete [] &s_Message;
//	}

	/* Member functions.*/
	void printMessage(){
		cout << "The input message is: " << s_Message <<endl;
	}
};

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
		//cout<<"Conversion complete"<<endl;
	 }
	 void printMessage()
	 {
	 	cout << "The input message is: " << inputString <<endl;
	 	cout << "The converted message is: " << outString<<endl;
	 }
	void sendToLights(char);
	void sendToSpeaker(char);
};

void MorseCodeMessage::sendToLights(char inchar)
{
	//Variable to hold Delay for generating a square wave for speaker
	float Time1 = 100000;
	float Time2 = 100000;
	//Open the memory file desciptor
	char *filename = "/dev/mem";
	int fd = open(filename, O_RDWR);
	//Map the physical address of the portA register to the memory
	int *portA_data = (int*)mmap(NULL, getpagesize(),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0x80840000);
	//Point to the PortB and PortF registers by offseting the
	//Port A pointer

	//Set the address of the data register of Port B
	int *portB_data = portA_data + 1;
	//Set the address of the direction register of Port B
	int *portB_ddr =  portA_data + 5;

	//Make the pins connected to the LED's on the aux board as output
	//by making them 1 in the direction register
	*portB_ddr |= 0x00E0;

	//Make all the IO pins (connected to LED and buttons as 0). The pins
	//which are configured as input are not affected by this
	(*portB_data) &= 0xFF00;

	if(inchar == '.')
	{
		int count = 0;
		while(count < 20)
		{
			//Make the IO pin connected to speaker as 1, do not change other pins.
			(*portB_data) |= 0xFF20; // red port -5 red
			//(*portF_data) |= 0x0002;
			if (msync(portA_data,getpagesize(), MS_SYNC) == -1)
				printf("Fail\n");
			usleep(Time1);
			(*portB_data) &= 0xFF00; // turn off
			//(*portF_data) |= 0x0002;
			if (msync(portA_data,getpagesize(), MS_SYNC) == -1)
				printf("Fail\n");
			usleep(Time1);
			count = count + 1;
		}
	}
	else
	{
		int count = 0;
		while(count < 20)
		{
			//Make the IO pin connected to speaker as 1, do not change other pins.
			(*portB_data) |= 0xFF40; // yellow port - 6 yellow
			//(*portF_data) |= 0x0002;
			if (msync(portA_data,getpagesize(), MS_SYNC) == -1)
				printf("Fail\n");
			usleep(Time1);
			(*portB_data) &= 0xFF00; // turn off
			//(*portF_data) |= 0x0002;
			if (msync(portA_data,getpagesize(), MS_SYNC) == -1)
				printf("Fail\n");
			usleep(Time1);
			count = count + 1;
		}
	}
}

void MorseCodeMessage::sendToSpeaker(char inchar)
{
		//Variable to hold Delay for generating a square wave for speaker
		float Time1 = 10000;
		float Time2 = 30000;
  		//Open the memory file desciptor
		char *filename = "/dev/mem";
		int fd = open(filename, O_RDWR);
		//Map the physical address of the portA register to the memory
		int *portA_data = (int*)mmap(NULL, getpagesize(),PROT_READ | PROT_WRITE,MAP_SHARED,fd,0x80840000);
		
		//Set the address of the direction register of Port F
		int *portF_ddr =  portA_data + 13;
		//Set the address of the data register of Port F
		int *portF_data =  portA_data + 12;
		unsigned int i= 0;

		//Make the pin connected to the Speaker on the aux board as output
		//by making it 1 in the direction register
		*portF_ddr |= 0x0002;
		//Make the IO pin connected to speaker as 0.
		(*portF_data) &= 0xFFFD;

		if(inchar == '.')
		{
			int count = 0;
			while(count < 20)
			{
				//Make the IO pin connected to speaker as 0, do not change other pins.
				(*portF_data) &= 0xFFFD;
				//Sync the pagesize data memory to the physical address of pins
				//by using msync function. If result is -1, the the data cannot be copied
				if (msync(portA_data,getpagesize(), MS_SYNC) == -1)
					printf("Fail\n");
				//Wait for generating a square wave
				usleep(Time1);

				//Make the IO pin connected to speaker as 1, do not change other pins.
				(*portF_data) |= 0x0002;
				//if (msync(portA_data,getpagesize(), MS_SYNC) == -1)
					//printf("Fail\n");
				usleep(Time1);
				count = count + 1;
			}
		}
		else
		{
			int count = 0;
			while(count < 20)
			{
				//Make the IO pin connected to speaker as 0, do not change other pins.
				(*portF_data) &= 0xFFFD;
				//Sync the pagesize data memory to the physical address of pins
				//by using msync function. If result is -1, the the data cannot be copied
				if (msync(portA_data,getpagesize(), MS_SYNC) == -1)
					printf("Fail\n");
				//Wait for generating a square wave
				usleep(Time2);

				//Make the IO pin connected to speaker as 1, do not change other pins.
				(*portF_data) |= 0x0002;
				//if (msync(portA_data,getpagesize(), MS_SYNC) == -1)
				//	printf("Fail\n");
				usleep(Time2);
				count = count + 1;
			}

		}
}

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
	/*~MessageStack(){
		string Stack = NULL;
	}*/

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
	void printStack(string printThis)
	{
		cout<<printThis<<endl;
	}

};

int main(void){

	MessageStack stack;
	MorseCodeMessage board;
	MorseCodeMessage morse;
	string tmp;
	int lengthOfString;

	morse.translateMessage2Morse();
	stack.PushData(morse.inputString);
	stack.PushData(morse.outString);

	for (int k=0; k<1;k++)
	{
		tmp = stack.PopData();
		cout<<tmp;
		lengthOfString = tmp.length();
		for(int n = 0;n<=lengthOfString;n++)
		{
			cout<<"I am sending -"<<tmp[n]<<"To speaker"<<endl;
			//board.sendToLights(tmp[n]);
			board.sendToSpeaker(tmp[n]);
		}

	}
	return 0;
}

