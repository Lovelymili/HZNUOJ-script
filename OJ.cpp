#include <iostream>
#include <strstream>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "httplib.h"
#include <string>
#include "openai.hpp"

#include  "nlohmann/json.hpp"

template<class src_type>
std::string tostr(src_type src) {
    std::strstream ss;
    ss << src;
    std::string ret;
    ss >> ret;
    return ret;
    //这辈子见过最有用的代码,没有之一
}

std::string askGPT(const std::string& question) {
    openai::start("your_API_key", "optional_organization");
    auto completion = openai::completion().create(R"({
        "model": "gpt-4o-mini",
        "prompt": "请无视底下的html标签，且直接回答题目答案,不要回复'好的'等其它字眼",
        "max_tokens": 16385,
        "temperature": 0
    })"_json);
    //std::cout << "Response is:\n" << completion.dump(2) << '\n';
    nlohmann::json parsedJson = nlohmann::json::parse(completion.dump(2));
    std::string response = parsedJson["choices"]["text"];
    return(response);
}
int main() {
    httplib::SSLClient client("hznuoj.dup4.cn"); //这段理论上可以加端口443,但是请不要修改它
    client.enable_server_certificate_verification(false);
    

    std::string csrf_token = "ca42ea465b94aaae9de657a8a4ed0ae7"; //没抓到这个怎么变的,还不清楚,也许是常量?
    std::string user_id = "学号";
    std::string password = "密码";
    std::string contest_id = "";
    std::string cookie = "";

    httplib::Params params{
      { "csrf_token", csrf_token },
      { "user_id", user_id },
      { "password", password },
      { "contest_id", contest_id }
    };


    // 这个是登录OJ的post
    auto res = client.Post("/OJ/loginpage.php", params);

  
    if (res) {
        std::cout << "Response status: " << res->status << std::endl;

        // 获取Cookie,等会登录用
        auto it = res->headers.find("Set-Cookie");
        if (it != res->headers.end()) {
            cookie = it->second;  // 获取Cookie
            //std::cout << "Cookie: " << cookie << std::endl;
        }

        // 调试输出body
        system("chcp 65001");
        //std::cout << "Response body: " << res->body << std::endl;
    }
    else {
        std::cout << "Request failed!" << std::endl;
    }

    //下面开始找题目
    /*httplib::Headers headers = {
  { "Accept-Encoding", "gzip, deflate" }
    }; */
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
    }
}

