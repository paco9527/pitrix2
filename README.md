Pitrix
=====================
这是一个基于树莓派3B + WS2812点阵屏方案实现的电子钟，可以编写Lua脚本获取所需信息并显示到点阵屏上，实现显示时间、联网获取天气等功能，而不需要连接到专门的服务端。

本仓库基于以下开源项目“焊接”而成：

- 点阵屏驱动依赖：
  - [jgarff/rpi_ws281x](https://github.com/jgarff/rpi_ws281x)
  - [WiringPi/WiringPi](https://github.com/WiringPi/WiringPi)
- 上层用户程序依赖：
  - [lua/lua](https://github.com/lua/lua)
  - [lvgl/lvgl](https://github.com/lvgl/lvgl)
  - [XuNeo/luavgl](https://github.com/XuNeo/luavgl)
  - [mpx/lua-cjson](https://github.com/mpx/lua-cjson)

## 使用方法

### 1. 获取源码
```shell
git clone https://github.com/paco9527/pitrix2.git
git submodule update --init # 目前不需要添加参数--recursive，编译luavgl时会使用extlib/lvgl，不需要下载额外的依赖
```

### 2. 编译
编译脚本为`makeall.sh`，用于在交叉编译环境下工作：
```shell
mkdir output
./makeall.sh arm-linux-gnueabihf- buildlib
```

编译完成后，下次编译可以省略`buildlib`参数
```shell
./makeall.sh arm-linux-gnueabihf-
```

清理编译文件使用命令
```shell
make clean
```

### 3. 在树莓派上执行
编译成果物为可执行文件`pitrix`；将`app`目录和`pitrix`文件拷贝到树莓派上，`app`目录和`pitrix`可执行文件在同一目录下，然后执行：
```shell
sudo ./pitrix
```

注意，rpi_ws281x库操作硬件需要使用root权限。执行命令后应当能看到`Enter Pitrix REPL.`的提示且没有异常退出，这时可以继续输入命令了。

### 4. 加载脚本
`app`目录下包含一些自测应用，以显示时间的应用为例，该应用的脚本在`app/time`目录下，输入命令：
```shell
sys.load("app/time")
```

如果输入参数正确，且输入的目录下存在`disp.lua`文件，程序会开始执行`disp.lua`脚本，点阵屏上会显示当前时间。

#### 目前支持的命令

##### sys
`sys.ls()`

列出当前已加载的应用，输出包括应用id和应用名：
```shell
sys.ls()
current loaded apps:
id: 0   name: app, node: 0x75c066e0
id: 1   name: app, node: 0x75c14100
id: 3   name: label, node: 0x75c22df8
```

`sys.load(path, name)`

从指定目录加载一个应用。

输入：
- path：应用脚本**所在目录**
- name：应用名，不填写时默认为"app"

`sys.del(id)`

输入：
- id：应用id，可以从`sys.ls`中查询需要删除的应用号

`sys.log_lvl(lvl)`

输入：
- lvl：打印等级，实现Pitrix的C代码内包含一些调试打印，小于输入等级的调试打印不会显示；打印等级从小到大分别为DEBUG-ERROR-INFO，默认等级为ERROR

`sys.set_brightness(brightness)`

输入：
- brightness：屏幕亮度，在rpi_ws281x这个驱动上支持的范围是0~255

补充
----------------

### 关于编译脚本...
*整个编译流程都挺糙的，脚本生成`-I`参数列表这个力大砖飞的路子我自己看着都头疼...*

shell脚本实现比较**糙**，主要分三步：
1. 生成`-I`包含路径参数供编译器使用
2. 单独编译第三方库
3. 编译其余代码

编译脚本有两个输入参数：
- 参数1：交叉编译工具名称前缀；
- 参数2：如果有`buildlib`参数，会重新编译WiringPi和rpi_ws281x两个库。

PS：目前WiringPi和rpi_ws281x两个库的编译流程是在调用make前执行的，编译流程参考自这些项目的README文档

### 希望使用其他屏幕驱动程序？
在core/modules/render/lv_adapter.c下有：
- 用于刷新屏幕的**硬件相关**代码
- 注册lvgl显示设备的代码

在core/modules/render/render.c下有函数`render_hardware_setting`，该函数会**调用硬件相关代码**来配置屏幕参数

如果需要增加硬件相关代码或更换extlib目录中的屏幕驱动`rpi_ws281x`，需要修改这两份源码文件来适配。


### 为什么这玩意儿名字里有2？
很久以前还有一个做得更糙的点灯板程序，使用预先编译好的动态库作为应用，然后整个运行流程差不多是这个样子：
- 主进程在加载动态库时，会尝试在动态库里查找`setup()`和`loop()`两个符号
- 找到符号后，用专门的线程函数给应用新建*线程*和线程*专用*的条件变量
  - `setup()`负责进入循环前的初始化工作，`loop()`在线程函数的循环里调用
  - 每次循环都会获取专用条件变量，其他线程负责切换应用，选中应用时定时发送该应用的条件变量，否则放任该应用阻塞

又是依赖“Pitrix内核”的动态库又是线程，实在繁琐，但当时哪里知道还有什么法子能做这个hhh

*自己焊接代码的水平有限，Lua用的也不算太熟，万一这堆代码真有人来看，不足之处欢迎路过的各位少侠指正*

