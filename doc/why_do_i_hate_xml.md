
为什么我不喜欢xml
=================================================

简单的说

* 可读性差
* 编程不友好

当然还有更细致的的约束, 比如仅有少量配置项的时候怎么怎么的, 用在什么什么场合的时候怎么怎么的, 但从内心深处, 我觉得大部分任何场景都不应该使用它, 适合使用xml的场景少到我觉得可以把它当作一个DSL来看.

### 可读性
xml本身是和

### 编程友好度

如果你喜欢xml, 请告诉我你是否使用了现成xml编码解码器? 如果没有现成的xml解码器, 自己手动编写代码读写xml文档, 大概要耗费多少时间?

我认识的xml爱好者(大多数是Java程序员), 喜欢说的话是: "用xml不就行了吗, 解析简单", 我想要是把xml解析的jar包从他们工程里删掉, 我估计他们再也不会想用xml了.

why?

因为通常情况下(注意是通常), xml的解析至少需要载入整个xml内容, 而且需要处理各种可能的嵌套. 所以无论是算法还是时间, 还是空间上, 
