 #include <iostream>
#include "http.h"
#include "iat.h"
#include "tts.h"
#include "myvlc.h"
#include <json/json.h>



using namespace std;

int main(int argc, char const *argv[])
{	
	int ret;

	Iat myIat;
	Http myHttp;
	Tts myTts;
	AudioPlay myAudioPlay;

	cout<<"语音交互系统"<<endl;
	cout<<"小灰灰：你好啊！我是小灰灰～～～"<<endl;
	cout<<"小灰灰：有什么需要帮助的呢？"<<endl;

	while(1)
	{
		ret = myIat.iatFunc();
		if(IAT_SUCCESS == ret)
		{

			if(myIat.iat.callflag == 1)
			{
				Json::Reader  reader ;
				Json::Value     value  ;

				char* str = (char*)malloc(BUFSIZE);
				memset(str, 0, BUFSIZE);

				string postData,infoMSG;
				infoMSG=myIat.iat.rslt;
				cout<<"我："<<myIat.iat.rslt<<endl;
				
				postData="key=84779b356eb9497d92ada64a70a3fb64&info="+infoMSG;
				char* postDataPoint  = &postData[0];

				if(myHttp.httpPost("http://www.tuling123.com/openapi/api", postDataPoint,str))
				{
					char* strPort = strchr(str, '{');
					if(reader.parse(strPort,value))
					{

						cout<<"小灰灰："<<value["text"].asString()<<endl;
						myTts.ttsFunc(&value["text"].asString()[0]);

						free(str);
					}
				} 
				myAudioPlay.playAudio("tts.wav");
			}
			else if(myIat.iat.callflag == 0 ||myIat.iat.callflag == 2)
			{
				myTts.ttsFunc(myIat.iat.rslt);
				myAudioPlay.playAudio("tts.wav");
			}
		}	
	}
		
	return 0;
}



