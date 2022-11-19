# SGL----一个简易的图形库

> 一个为小屏幕设计的一个图形库,旨在使用起来开放,简单

## [v0.1] 2022/11/19

- sgl:负责总handle,配置屏幕,开启DEBUG各种参数,支持裁剪增加各种控件
- sgl_obj:负责完成对象链表实现,可以删除,创造,设置对象
- sgl_anim:负责完成动画链表的实现,负责创造动画,实现对象效果
- sgl_draw:负责实现绘制函数,对绘制显存进行绘制操作
- sgl_driver:负责实现图像库对底层函数对接的接口