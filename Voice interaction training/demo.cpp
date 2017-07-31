 #include <iostream>
#include <string>

#include "turing.h"
#include "IAT.h"
#include "TTS.h"

using namespace std;

int main(int argc, char const *argv[])
{
	
	
	IAT one;

	cout<<"语音交互系统"<<endl;
	cout<<"--------------------语音技术由科大讯飞提供"<<endl;
	cout<<"小灰灰：你好啊！我是小灰灰～～～"<<endl;
	cout<<"小灰灰：有什么需要帮助的呢？"<<endl;

	while(1)
	{
          		one.iatFunc();
	}	
	
	return 0;
}



