# tiny_OS
A TINY OPERATING SYSTEM..233333
玩具，玩具............

目前只能引导,在屏幕上打印..



不过最起码制作成了一个镜像(雾



## 怎么玩？
**首先安装虚拟机 推荐使用i386的各个Linux编译,x64平台上有可能编译不通过.安装qemu**
退一步讲要运行它也必须使用i386 啊...

debian:
```
sudo apt-get install qemu build-essential
```

------------------------

```
git clone https://github.com/windski/tiny_OS.git
make
make run_qemu
```
