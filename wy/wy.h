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

 *@name:        wy.h
 *@description: wy main head file
 *@author:      yuanshuai
 *@mail:        446968454@qq.com
 *@date:        2020-12-19 09:49:51
 *@version:     V1.0.0
*/

#ifndef WY_H 
#define WY_H 

struct HandlerEvent {
    /* data */
};

struct wyEventCenter {
    int nMaxfd;      // 最大文件描述符
    int nHighlimit;  // 文件描述符上限
    
};

wyEventCenter *wyCreateEventCenter(int nHighlimit);

#endif	// WY_H