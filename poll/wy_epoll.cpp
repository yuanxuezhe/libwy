/*
 * 
 *       ┌──┘ ┴───────┘ ┴──┐
 *       │                 │
 *       │       ───       │
 *       │  ─┬┘       └┬─  │
 *       │                 │
 *       │       ─┴─       │
 *       │                 │
 *       └───┐         ┌───┘
 *           │         │  神兽保佑
 *           │         │  永无BUG
 *           │         └──────────────┐
 *           │                        ├─┐
 *           │                        ┌─┘   
 *           │                        │
 *           └──┐ ┐ ┌─────────┬─┐ ┌───┘         
 *              │─┤─┤         │─┤─┤         
 *              └─┴─┘         └─┴─┘ 

 *@name:        wy_epoll.cpp
 *@description: epoll
 *@author:      yuanshuai
 *@mail:        446968454@qq.com
 *@date:        2020-12-19 10:16:55
 *@version:     V1.0.0
*/

#include <sys/epoll.h>
#include <vector>

typedef std::vector<struct epoll_event> EventList;

static int wyCreate