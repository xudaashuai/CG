使用指南

控制台 输入 ./g < cube.obj 或者 ./g cube.obj

按键功能

C  切换颜色
X	随机颜色
+  放大
-	缩小
方向键 移动光源位置
R  自动旋转
T  开启网格简化



鼠标

按下左键拖动旋转模型
按下右键拖动移动模型

介绍

使用了实现了DISPLAY LIST，但是为了展示网格简化并没有使用。
为了方便看出网格简化的效果，画的线模型。
glut没有滚轮，于是用+-控制缩放。
glut真的有点老旧，正在想自己oo简单封装，但是放弃了。
论文里的网格简化算法实现不来，自己简单想了一个，思路是每次去除最小的那条边，用边的中点代替边的端点。

已知bug

在过度缩放或平移时，会出现模型部分不显示或者完全不显示。
旋转时背面亮度低（我小小的加强了一下环境光 ：-））。

帮助来源：

The OpenGL Utility Toolkit (GLUT) Programming Interface API Version 3 --- https://www.opengl.org/resources/libraries/glut/spec3/spec3.html
我的博客 - www.xudashuai.top 或者 xidaashuai.github.io 