# system programming


###Record of the experimental process





gcc 常用指令
```bash
$ gcc [options] <inputs>    

-o：指定生成的输出文件；
-E：仅执行编译预处理；
-S：将C代码转换为汇编代码；
-wall：显示警告信息；
-c：仅执行编译操作，不进行连接操作。

```

objdump 常用指令
```bash
$ objdump -d test    
反汇编test中的需要执行指令的那些section    

$ objdump -D test    
与-d类似，但反汇编test中的所有sectiont    

$ objdump -h testt    
显示test的Section Header信息

$ objdump -x testt    
显示test的全部Header信息

$ objdump -s testt    
除了显示test的全部Header信息，还显示他们对应的十六进制文件代码
```

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
