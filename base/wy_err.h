#ifndef WY_ERR_H 
#define WY_ERR_H 

#include <stdio.h>
#include <stdlib.h>

#define ERR_EXIT(m) \
        do \
        { \
            perror(m); \
            exit(EXIT_FAILURE); \
        } while (0);
        

#endif	// WY_ERR_H