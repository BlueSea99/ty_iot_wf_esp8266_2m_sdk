1、编译方法：
顶级目录下  ./build_app.sh 应用路径 应用名称 应用版本 应用存储大小 应用类型 编译文件存储目录
举例：./build_app.sh demos/product product 1.0.0  2M debug output/dist/product_x.x.x
2、SDK目录介绍
app_components：应用组件目录 
apps：应用程序目录，应用程序目录下需包括user(*.c)和include(*.h)文件夹,user文件夹下需有Makefile（可参考demo程序）
demos：例程应用目录
platforms：平台编译工具及脚本
scripts：各类工具
sdk：库文件及头文件