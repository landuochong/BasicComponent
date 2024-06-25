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

### 事件总线：**EventBus**

Handler和Event是相对应的

```c++
class CustomEvent : public Event
{
public:
  CustomEvent() :
};

class CustomListener : public EventHandler<CustomEvent>
{
public:
  virtual void onEvent(CustomEvent & e) override {
    //处理事件
  }
};

class CustomListener2 : public EventHandler<CustomEvent>
{
public:
  virtual void onEvent(CustomEvent & e) override {
    //处理事件,不要做耗时操作，最好在子线程处理，因为是引用，切换线程可能需要copy。
  }
};

CustomListener listener;
//添加针对事件CustomEvent的handler
EventBus::AddHandler<CustomEvent>(&pListener);
//支持多个Listener
CustomListener2 listener2;
EventBus::AddHandler<CustomEvent>(&pListener2);

//移除handler
EventBus::RemoveHandler<CustomEvent>(&pListener);
EventBus::RemoveHandler<CustomEvent>(&pListener2);

//发送事件
CustomEvent event;
EventBus::SendEvent(event);
```



### utils工具类

​	strutil：字符串相关

​	time_utils:时间相关

### 测试demo

 	app目录下测试代码 

​         QT打开Test.pro即可编译运行







依赖库：加密依赖OpenSSL

### 第三方：

```wiki
abseil-cpp：
https://github.com/abseil/abseil-cpp.git@c8a2f92586fe9b4e1aff049108f5db8064924d8e

sigslot:
https://sigslot.sourceforge.net/


```



下载代码：

创建BasicComponent目录

1.复制doc目录下的 .gclient和.gclient_entries，放到目录下

2.执行gclient sync，会下载项目源码和第三方依赖

3.src下的代码就是我们的项目代码



参考文档：

1.线程安全：https://gitee.com/l0km/common_source_cpp.git

2.