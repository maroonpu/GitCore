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

		// string infoMSG,postData;
		// cout<<"你：";
		// cin>>infoMSG;	//需要查资料优化
		// postData=postDataPref+infoMSG;
		// char* postDataPoint ;
		// postDataPoint = &postData[0];
		// if(tur->HttpPost("http://www.tuling123.com/openapi/api", "key=84779b356eb9497d92ada64a70a3fb64&info=",str))
		// {
		// 	char* strPort = strchr(str, '{');
		// 	if(reader.parse(strPort,value))
		// 	{
		// 		cout<<value["text"].asString()<<endl;
		// 	}

		// } 
	}	
	
	
	
	return 0;



}



