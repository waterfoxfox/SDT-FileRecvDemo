# DEMO说明
该DEMO接收指定服务器、指定房间和位置的音视频流，其中视频流写h264文件。本DEMO可作为纯播放端API调用的参考素材。<br>

该DEMO的配置文件TerminalSdkTest.ini中可设置服务器IP、房间ID、位置信息，同时还可以设置是否保存接收数据到文件。


```js
[Config]
;服务器IP
ServerIp=47.106.195.225
;服务器域号
DomainId=3
;房间号
RoomId=888
;接收位置
DownPosition=1
;是否保存接收视频流到文件
SaveRecvData=1
```
<br>

SDK API的调用集中在SDClient.cpp中
<br>



测试工程使用VS2010或更高版本编译



---

# 相关资源
跟多文档、代码资源见：https://mediapro.apifox.cn

SDK 商用及定制化、技术支持服务可联系：[http://www.mediapro.cc/](http://www.mediapro.cc/)

