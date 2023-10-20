## C++基础组件(跨平台)

### json类

​	json相关，json.h来自第三方库：https://github.com/nlohmann/json

### crypt加密类

​	sha256，base64， pkcs7

### thread线程相关

​	thread：参考webrtc写的线程类

​	ThreadPool：线程池类

### timer定时器

​	Timer：定时回调类

​	TaskTimer：定时任务类

### utils工具类

​	strutil：字符串相关

​	time_utils:时间相关

### 测试demo

 	app目录下测试代码 

​         QT打开Test.pro即可编译运行







依赖库：加密依赖OpenSSL

下载代码：

1.下载.gclient和.gclient_entries，放到目录下

2.执行gclient sync



参考文档：

1.线程安全：https://gitee.com/l0km/common_source_cpp.git

2.