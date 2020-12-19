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

 *@name:        server.h
 *@description: server
 *@author:      yuanshuai
 *@mail:        446968454@qq.com
 *@date:        2020-12-19 12:28:27
 *@version:     V1.0.0
*/

#ifndef SERVER_H 
#define SERVER_H 

#include "wy/wy.h"
#include <string.h>

class CYsServer {
public:
    char *szBindaddr;         /*监听地址*/
    int nPort;              /*监听端口*/
    wyEventCenter *sEc;
};

#endif	// SERVER_H