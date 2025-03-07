#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

const int Creater = 1;
const int User = 2;
const char *Gpath = "/home/Jiasty/GithubRep/Meetlinux/OSCode/16.shm"; // 用于生成共享内存键值的文件路径。
const int Gproj_id = 0x66;
const int GshmSize = 4096; // 建议4096的整数倍字节

class Shm
{
private:
    key_t Getkey()
    {
        key_t key = ftok(Gpath, Gproj_id); // 用户层唯一标识符，使两个进程看到同一个共享内存。
        if (key < 0)
        {
            perror("ftok");
        }
        return key;
    }

    int GetshmHelper(key_t key, size_t size, int shmflg)
    {
        int shmid = shmget(key, size, shmflg); // 返回值为共享内存段的标识码。内核层唯一标识
        if (shmid < 0)
        {
            perror("shmget");
        }
        return shmid;
    }

    std::string RoleToString(int who)
    {
        if (who == Creater)
            return "Creater";
        else if (who == User)
            return "User";
        else
            return "None";
    }
    void *ShmAttach() // 申请了一段共享内存,挂接到当前进程地址空间
    {
        void *ret = shmat(_shmid, _addrshm, 0);
        if (ret == nullptr)
        {
            perror("shmat");
            return nullptr;
        }
        std::cout << "who:" << RoleToString(_who) << "attach shm..." << std::endl;
        return ret;
    }

public:
    Shm(const std::string path, int proj_id, int who)
        : _path(path), _proj_id(proj_id), _who(who), _addrshm(nullptr)
    {
        _key = Getkey();
        if (_who == Creater)
            GetshmforCreater(Creater);
        else if (_who == User)
            GetshmforUser(User);
        _addrshm = ShmAttach();

        std::cout << "shmid: " << _shmid << std::endl;
        std::cout << "_key: " << _key << std::endl;
    }
    bool GetshmforCreater(int who)
    {
        if (who == Creater)
        {
            _shmid = GetshmHelper(_key, GshmSize, IPC_CREAT | IPC_EXCL | 0666); // 创建共享内存段
            if (_shmid >= 0)
            {
                std::cout << "shm create done..." << std::endl;
                return true;
            }
        }
        return false;
    }

    bool GetshmforUser(int who)
    {
        if (who == User)
        {
            _shmid = GetshmHelper(_key, GshmSize, IPC_CREAT | 0666); // 获取已存在的共享内存段。
            if (_shmid >= 0)
            {
                std::cout << "shm get done..." << std::endl;
                return true;
            }
        }
        return false;
    }

    void CleanShm()
    {
        if (_addrshm)
            memset(_addrshm, 0, GshmSize);
    }

    void *Addr()
    {
        return _addrshm;
    }

    ~Shm()
    {
        if (_who == Creater)
        {
            shmctl(_shmid, IPC_RMID, nullptr);
        }
        std::cout << "shm remove done..." << std::endl;
    }

private:
    key_t _key; // 共享内存段名字,具唯一性
    int _shmid;
    std::string _path; // 共享内存连接到的地址空间起始地址。
    int _proj_id;      // 项目ID，用户制定。
    int _who;          // 使用身份
    void *_addrshm;    // 记录申请到的共享内存的地址。
};