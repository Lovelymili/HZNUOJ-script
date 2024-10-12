# HZNUOJ-script 简介

HZNUOJ-script，亦可称 `HZNU-AutoOJ`。是一个基于 [c++](https://www.cnblogs.com/lyxtech/articles/15203780.html) 由匿名开发者`Lovelymili`随手瞎写的 HZNUOJ 自动刷题脚本。

## 特色 

**高效·快速**

1. 理论支持题库内所有题型
2. 回复基于大语言模型,准确率高
3. 支持使用自有模型,支持使用镜像 

## 注意事项

>> 首先请确保您的Visual Studio或其它IDE中项目配置C++版本为 11 / 14 不能使用17及以上版本

>> 然后确保您已安装OpenSSL 3.0以上版本 （多数情况下Linux系统自带为1.0,Windows为1.1）


## 使用方式

**修改自己的学号,密码等数据**

分别是
第`40`行的`user_id`
第`41`行的`password`
第`22`行的`your_API_key`和`optional_organization`

**修改下列代码**
这里的循环由于时间不够没来得及破译每个章节和题目对应ID是什么，有时间可能会回来补上这段a,b的赋值

``` cpp
 for (int a = 0, b = 0;; a++, b++) //这里需要自己修改一下逻辑,这里仅供参考
 {
     auto res2 = client.Get("/OJ/problem.php?cid=" + tostr(a) + "&pid=" + tostr(b));
     //std::cout << "Response body: " << res2->body << std::endl;
     std::string body1 = tostr(res2->body);
     //构建问题模板
     std::string question = body1.substr(body1.find_first_of("Description") - 1, body1.find_first_of("Input"));
     std::string tips = body1.substr(body1.find_first_of("Hint") - 1, body1.find_first_of("<!-- gptcode start -->"));
     std::string newquestion = "问题是:" + question + "提示是:" + tips;
     //问ChatGPT,默认是4o-mini,需要可以自己修改,如果需要使用中转的话修改openai.hpp
     std::string response = askGPT(newquestion);
     //submit答案
     httplib::Params params1;
     params1.emplace("source", response);
     params1.emplace("cid", tostr(a));
     params1.emplace("pid", tostr(b));
     client.set_keep_alive(true);
     httplib::Headers headers = {
        { "Cookie", "lastlang=1; " + cookie}
     };
     auto res3 = client.Post("/OJ/submitpage.php?cid=" + tostr(a) + "&pid=" + tostr(b) + "&langmask = 4121599", headers, params1); //langmask是根据你自己写的语言定的,这里写的是c++11
     return 0;
```

**尝试运行一下吧**

## 补充说明

我觉得这个项目其实拿python写比拿c++写好的多的多的多，不过你可以把它交给AI翻译成python代码

本项目欢迎大佬fork和提出pr 顺便帮忙解决一下上述的a,b赋值问题

如果使用自己的模型的话，需要遵守openai调用规则,或者大哥自己搓个对接自己模型的库，一般fastgpt,lanlangchat都是可以的,chatglm等部分模型自带了有关API

如果需要调整链接,尝试修改`openai.hpp`,这个链接不在`OJ.cpp`中
