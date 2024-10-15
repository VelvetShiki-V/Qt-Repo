# Utilities指南

v1.2.0

2024-10-13 宋峰

## 简介

Utilities是用于Qt项目的内部常用功能库。

## 组成

Utilities包括了以下常用功能

- jobject

  读写JSON内存对象

- jfile

  读写JSON文件

- qlog

  写日志
  
- qpy

  运行python程序
  
  

依赖关系图如下图所示：

![utilities](C:\Users\Fowle\Desktop\系统\utilities.png)

## JObject用法

Qt程序开发中通常采用QJsonObject等类来完成各种JSON数据操作。

utlities在QJsonObject等类的基础上封装了JObject类来实现更简单快捷的处理。

JObject包括了jobject.h和jobject.cpp文件。



JObject的常见操作如下所列：

- 创建JObject对象

创建一个空JObject对象（这意味着存储了空的JSON数据:{}）

```
  JObject jo = JObject();
```

或者根据给出的JSON字符串创建一个JObject对象

```
  QString js = "{\"name\":\"david\",\"age\":17,\"male\":true}";
  JObject jo = JObject(js);
```

jo是一个JObject对象，存储了JSON数据

```json
{
    "name":	"david",
    "age": 17,
    "male": true
}
```



- 读取JObject对象全部内容

```
    qDebug() << jo.text();
-------------------------------------------------------------------------------
"{\n    \"age\": 17,\n    \"male\": true,\n    \"name\": \"david\"\n}\n"
```

- 检查JObject对象中某个元素是否存在，及其值类型

  ```
  qDebug() << jo.check("unknown");
  qDebug() << jo.check("name");
  qDebug() << jo.check("age");
  qDebug() << jo.check("male");
  -------------------------------------------------------------------------------
  128
  3
  2
  1
  ```

  键"unknown"并不存在，返回的是枚举值128（*QJsonValue::Undefined*）！

  键“name","age","male"真实存在，分别返回3（*QJsonValue::String*），2（*QJsonValue::Double*），1（*QJsonValue::Bool*）。

  

- 解析JObject对象的元素

```
    qDebug() << jo.parse("name").toString();
    qDebug() << jo.parse("age").toInt();
    qDebug() << jo.parse("male").toBool();
-------------------------------------------------------------------------------
"david"
17
true
```

- 解析JObject对象的内嵌元素

  内嵌元素的键用"."分割表示

```
    QString js = "{\"parents\":{\"father\":\"Tim\",\"mother\":\"Grace\"}}";
    JObject jo = JObject(js);
    qDebug() << jo.parse("parents.father").toString();
-------------------------------------------------------------------------------
"Tim"

```

​		对于数组元素的键同样可以用"."分割，不同的是用数组元素的索引表示，如下所示

```
    QString js = "{\"members\":[\"Jana\",\"Cindy\",\"Zoey\"]}";
    JObject jo = JObject(js);
    qDebug() << jo.parse("members.0").toString();
    qDebug() << jo.parse("members.1").toString();
-------------------------------------------------------------------------------
"Jana"
"Cindy"

```

- 向JObject对象插入元素

```
	QString js = "{\"name\":\"david\"}";
	JObject jo = JObject(js);
    qDebug() << jo.upsert("age", 17);
    qDebug() << jo.upsert("male", true);
    qDebug() << jo.text();
-------------------------------------------------------------------------------
true
true
"{\n    \"age\": 17,\n    \"male\": true,\n    \"name\": \"david\"\n}\n"
```

- 向JObject对象插入内嵌元素

```
    QString js = "{\"parents\":{\"father\":\"Tim\"}}";
    JObject jo = JObject(js);
    qDebug() << jo.upsert("parents.mother", "Grace");
    qDebug() << jo.text();
-------------------------------------------------------------------------------
true
"{\n    \"parents\": {\n        \"father\": \"Tim\",\n        \"mother\": \"Grace\"\n    }\n}\n"

```

也可以向Object数组插入内嵌元素

```
    QString js = "{\"members\":[\"Jana\",\"Cindy\"}}";
    JObject jo = JObject(js);
    qDebug() << jo.upsert("members.2","Zoey");

```

- 更新JObject对象的元素

​		和插入元素是相同的操作手法。

```
	QString js = "{\"name\":\"david\",\"age\":17,\"male\":true}";
	JObject jo = JObject(js);
    qDebug() << jo.upsert("name", "John");
    qDebug() << jo.text(); 
-------------------------------------------------------------------------------
true
"{\n    \"age\": 17,\n    \"male\": true,\n    \"name\": \"John\"\n}\n"
```

- 删除JObject对象的元素

```
	QString js = "{\"name\":\"david\",\"age\":17,\"male\":true}";
	JObject jo = JObject(js);
    qDebug() << jo.remove("male");
    qDebug() << jo.text();    
-------------------------------------------------------------------------------
true
"{\n    \"age\": 17,\n    \"male\": true\n}\n"
```

- 删除JObject的内嵌元素

```
    QString js = "{\"parents\":{\"father\":\"Tim\",\"mother\":\"Grace\"}}";
    JObject jo = JObject(js);
    qDebug() << jo.remove("parents.father");
    qDebug() << jo.text();
-------------------------------------------------------------------------------
true
"{\n    \"parents\": {\n        \"mother\": \"Grace\"\n    }\n}\n"
```

​		对于将要删除的数组元素的键同样可以用"."分割，不同的是用数组元素的索引表示，如下所示

```
    QString js = "{\"members\":[\"Jana\",\"Cindy\"]}";
    JObject jo = JObject(js);
    qDebug() << jo.remove("members.1");
    qDebug() << jo.text();
-------------------------------------------------------------------------------
true
"{\n    \"members\": [\n        \"Jana\"\n    ]\n}\n"
```

## JFile用法

JFil包括了jfile.h和jfile.cpp文件，它e继承自JObject，实现了对JSON文件的直接存取操作。



- 创建JFile对象

  ```
      JFile jf("d:\\temp\\test.txt");
      qDebug() << jf.upsert("name","Bob");
      qDebug() << jf.upsert("age",17);
      qDebug() << jf.text();
  -------------------------------------------------------------------------------
  true
  true
  "{\n    \"age\": 17,\n    \"name\": \"Bob\"\n}\n"
  ```

  打开文件test.txt，可以看到JSON数据写入了

  ![test_jfile](C:\Users\Fowle\Desktop\系统\test_jfile.png)



JFile的存取操作和JObject相同。

## QLog用法

日志在软件的测试和监控中具有不可替代的作用。

QLog是一个日志模块，包括了文件qlog.h和qlog.cpp。

QLog模块的功能和特征包括：

- [x] 行日志

- [x] QObject成员函数的日志收集

- [x] QObject成员信号的日志收集

- [x] 基于类的日志文件

- [ ] 基于环的日志缓冲（待开发）

- [ ] 远程测试与网络存储（待开发）

  



QLog模块用于QObject及其派生类的测试，通过一个例程，简单说明QLog的常见用法。例程以MainWindow类为测试对象。

"mainwindow.h":

```c++
#include "qlog.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void test_log_1();	// test routine 1
    void test_log_2();	// test routine 2

    Q_INVOKABLE int f_add(int a, int b);
private:
    Ui::MainWindow *ui;
    QLog *log;
};

```

"mainwindow.cpp":

```c++
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , log(new QLog)
{
    ui->setupUi(this);
    log->setupLog(this, true, WARN);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete log;
}

int MainWindow::f_add(int a, int b)
{
    return a + b;
}

void MainWindow::test_log_1()
{
    log->dLine("test debug log", 10, 3.14, true);
    log->wLine("test warn log", 80, -3.14, false);
}

void MainWindow::test_log_2()
{
    int r;
    int a = 100;
    int b = 200;
    log->loggedCall("f_add", r, a, b);
}

```

"mainwindow.h"添加的内容：

1. 添加头文件"qlog.h"

   ```c++
   *#include "qlog.h"*
   ```

   

2. 在MainWindow类中添加一个私有QLog对象指针

   ```c++
   private:
   	QLog *log;
   ```

3. 在MainWindow类的构造函数中添加对QLog对象的初始化

   ```c++
   MainWindow::MainWindow(QWidget *parent)
       : QMainWindow(parent)
       , ui(new Ui::MainWindow)
       , log(new QLog)
   {
       ui->setupUi(this);
       log->setupLog(this, true, WARN);
   }
   ```

   日志文件名称是<log对象的ObjectName>.txt，但如果log对象没有定义ObjectName，则以其类名称代之，这里就是"MainWindow.txt"，日志文件目录是可执行程序的同目录。

   

   关于函数setupLog的参数，

   on_signals=true意味着收集MainWindow实例成员发出的的信号;

   level=WARN意味着低于WARN级别的行日志不被打印；

4. 在MainWindow类中添加被测试成员函数

   ```c++
       Q_INVOKABLE int f_add(int a, int b);
   ```

   被测的函数冠以宏*Q_INVOKABLE*！这意味着f_add可作为函数参数在外部被调用。

   f_add是一个简单的求和函数

   ```c++
   int MainWindow::f_add(int a, int b)
   {
       return a + b;
   }
   ```

   

现在，让我们测试一下日志功能吧。		

在MainWindows类中添加2个测试用例函数 test_case_1, test_case_2

```c++
void MainWindow::test_case_1()
{
    log->dLine("hello", 10, 3.14, true);
    log->wLine("hello", 80, -3.14, false);
}

void MainWindow::test_case_2()
{
    int sum;
    int a = 100;
    int b = 200;
    log->invoke("f_add", sum, a, b);
}

```

```c++
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    w.test_case_1();
    w.test_case_2();

    return a.exec();
}

```

运行这两个用例，打开日志文件"MainWindow.log"，我们可以看到：

![屏幕截图 2024-01-16 103958](C:\Users\Fowle\Pictures\Screenshots\屏幕截图 2024-01-16 103958.png)

log->dLine行打印因低于指定日志级别(DEBUG < WARN)而未执行；

log->wLine行打印执行了。

同时 f_add函数执行过程中的首尾行打印也执行了。

除了这两部分行日志之外，其他的行日志都是关于MainWindow的成员信号的。

## QPy用法

QPy是一个运行python程序的模块，包括了文件qpy.h和qpy.cpp。

QPy模块的核心是一个函数：

```c++
bool runPython(const QString interp, const QString wdir, const QString script, const int ts_ms, const QJsonObject input, QJsonObject& output, QLog &logger);
```

关于参数：

- interp - python解释器，缺省就写”python“
- script - 被执行的python程序
- wdir - script的路径
- ts - python程序最大时延，单位秒
- input - 输入的参数（JSON格式）
- output - 返回的参数（JSON格式）
- logger - 日志记录器



被调用的python程序的例子如下

```python
import sys
import json
import logger

def foo(req):
    return  {"status": "success",  "result": req}

if __name__ == "__main__":
    logger.start('debug')
    logger.ILine("script begin")
    for line in sys.stdin:
        try:
            data = json.loads(line.strip())
            response = foo(data)
            print(json.dumps(response))
            sys.stdout.flush()
            logger.ILine("script end")
        except Exception as e:
            logger.ELine(f"exception: {e}")
```

它通过标准输入和输出与宿主程序通信。
