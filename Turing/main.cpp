#include <iostream>
#include <string>
#include <json/json.h>
#include "HttpRequest.h"

using namespace std;

int main()
{
	Json::Reader  reader ;
	Json::Value     value  ;

	HttpRequest* Http = new HttpRequest;
	
	char* str = (char*)malloc(BUFSIZE);
	memset(str, 0, BUFSIZE);

	cout<<"小灰灰：你好啊！我是小灰灰～～～"<<endl;
	cout<<"小灰灰：有什么需要帮助的呢？"<<endl;

	string postDataPref = "key=84779b356eb9497d92ada64a70a3fb64&info=";

	while(1)
	{
		string infoMSG,postData;
		cout<<"你：";
		cin>>infoMSG;	//需要查资料优化
		postData=postDataPref+infoMSG;
		char* postDataPoint ;
		postDataPoint = &postData[0];
		if(Http->HttpPost("http://www.tuling123.com/openapi/api", postDataPoint,str))
		{
			char* strPort = strchr(str, '{');
			if(reader.parse(strPort,value))
			{
				cout<<value["text"].asString()<<endl;
			}

		} 
		else 
		{
			printf("请求失败!\n");
		}

	}	
	
	
	free(str);
	return 0;
}


