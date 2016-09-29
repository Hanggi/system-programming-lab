# system programming

[![gitter][gitter-image]][gitter-url]
  [![NPM version][npm-image]][npm-url]
  [![build status][travis-image]][travis-url]
  [![Test coverage][coveralls-image]][coveralls-url]
  [![OpenCollective Backers][backers-image]](#backers)
  [![OpenCollective Sponsors][sponsors-image]](#sponsors)

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

$ objdump -h test    
显示test的Section Header信息

$ objdump -x test    
显示test的全部Header信息

$ objdump -s test    
除了显示test的全部Header信息，还显示他们对应的十六进制文件代码
```

readelf 常用指令
```bash
-a
--all 显示全部信息,等价于 -h -l -S -s -r -d -V -A -I.

-h
--file-header 显示elf文件开始的文件头信息.

-l
--program-headers
--segments 显示程序头（段头）信息(如果有的话)。

-S
--section-headers
--sections 显示节头信息(如果有的话)。

-g
--section-groups 显示节组信息(如果有的话)。

-t
--section-details 显示节的详细信息(-S的)。

-s
--syms
--symbols 显示符号表段中的项（如果有的话）。

-e
--headers 显示全部头信息，等价于: -h -l -S

-n
--notes 显示note段（内核注释）的信息。

-r
--relocs 显示可重定位段的信息。

-u
--unwind 显示unwind段信息。当前只支持IA64 ELF的unwind段信息。

-d
--dynamic 显示动态段的信息。

-V
--version-info 显示版本段的信息。

-A
--arch-specific 显示CPU构架信息。

-D
--use-dynamic 使用动态段中的符号表显示符号，而不是使用符号段。

-x
--hex-dump= 以16进制方式显示指定段内内容。number指定段表中段的索引,或字符串指定文件中的段名。

-w[liaprmfFsoR] or --debug-dump[=line,=info,=abbrev,=pubnames,=aranges,=macro,=frames,=frames-interp,=str,=loc,=Ranges] 显示调试段中指定的内容。

-I
--histogram 显示符号的时候，显示bucket list长度的柱状图。

-v
--version 显示readelf的版本信息。

-H
--help 显示readelf所支持的命令行选项。

-W
--wide 宽行输出。 @file 可以将选项集中到一个文件中，然后使用这个@file选项载入。
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
