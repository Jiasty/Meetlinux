#pragma once
#include <memory>
#include "IOService.hpp"
#include "Protocol.hpp"

// 未来将request将变为response即可。
class NetCalcu
{
public:
    NetCalcu(/* args */)
    {
    }
    ~NetCalcu()
    {
    }

    std::shared_ptr<Response> Calculator(std::shared_ptr<Request> req)
    {
        auto response = Factory::BuildResponseDefault();
        switch (req->Oper())
        {
        case '+':
            response->Setret(req->X() + req->Y());
            break;
        case '-':
            response->Setret(req->X() - req->Y());
            break;
        case '*':
            response->Setret(req->X() * req->Y());
            break;
        case '/':
            if (req->Y() == 0)
            {
                response->Seterrno(1);
                response->Setdesc("div zero!");
            }
            else
            {
                response->Setret(req->X() / req->Y());
            }
            break;
        case '%':
            if (req->Y() == 0)
            {
                response->Seterrno(2);
                response->Setdesc("mod zero!");
            }
            else
            {
                response->Setret(req->X() % req->Y());
            }
            break;
        default:
        {
            response->Seterrno(3);
            response->Setdesc("illegal operation!");
        }
        break;
        }

        return response;
    }

private:
    /* data */
};