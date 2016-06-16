# ImageProcess

基于OPENCV和QT的跨平台图像对比、处理工具。
可用于屏幕截图，图像对比，屏幕图像查找等。

/*********************************************************************/
* @ FXQA Image Process: Ver.2.1.
* @ OpenCV: Ver.2.4.3. QT5.0.1. FreeImage3.16.0
* @ Author: xiaoxia_yu.
* @ Date: 2013/10/24.
* *********************************************************************/
* Function: Image and pdf compare.
* @ Parameter Description:
*   -c: Compare image or pdf. After -c should be followed by two files. Essentia
l.
*   -f: To force uniform size.
*   -i: Combine image use with -o
*   -n: Re-set the image size.Rules(x, y, width, height), By * separated.
*   -p: Image mode. 0 to set grey images, 1 to set color images.
*   -e: Comparison accuracy. Default is 0.
*   -a: Different adjust.
*   -o: Output images.
*   -s: Show images.
*   -h: Show This.
* @ eg. -c 1.png 2.png -n "20*30*500*600" -p 1 -e 2 -o 3.png -s
* *********************************************************************/
* Function: ScreenShot.
* @ Parameter Description:
* @ -g: Set to screen shot mode. Essential.
* @ -c: Compare image after screen shot, Parameters refer to the above.
* @ -w: To a screenshot of the application class name.
* @ -m: Re-set the screen shot size.Rules(x, y, width, height), By * separated.D
efault is full screen.
* @ -o: Output images.
* @ eg. -g -o e:\screenshot.png
* @ eg. -g -c E:\3.png -w AcrobatSDIWindow -m 10*10*700*800 -n 10*10*10*10 -o E:
different.png -s
* @ eg. -g -c -w App0ClassOrTitle*App1ClassOrTitle -m 10*10*700*800*20*20*700*80
0 -n 10*10*200*400 -o e:\different.png
/*********************************************************************/
