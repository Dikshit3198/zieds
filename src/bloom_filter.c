/* 
*   Zieds - A C implementation of a bloom filter
*   Copyright (C) <2016>  Grif Hughes - ghughes@smu.edu
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../include/bloom_filter.h"
#include "hashes.h"

#define G(arr, scalar, pointer, size) ((abs(arr[0] + scalar * FNV1A_Hash_WHIZ((void *)pointer, sizeof(int32_t)))) % size)

struct bloom_filter *bloom_init() 
{
        struct bloom_filter *bf = ALLOC_FILTER;
        if(bf == NULL) {
                perror("Alloc failed, exiting...");
                exit(ALLOC_ERR);
        }

        bf->nelems = 100;
        bf->mbits = bf->nelems * (sizeof(uint32_t) << 3);
        bf->nh = (bf->mbits / bf->nelems) * log(2); 

        bf->buf = ALLOC_FILTER_BUF(bf->nelems);
        if(bf->buf == NULL) {
                free(bf);
                perror("Alloc failed, exiting...");
                exit(ALLOC_ERR);
        }

        return bf;
}

struct bloom_filter *bloom_init_cap(uint32_t cap)
{
        struct bloom_filter *bf = ALLOC_FILTER;
        if(bf == NULL) {
                perror("Alloc failed, exiting...");
                exit(ALLOC_ERR);
        }

        bf->nelems = cap;
        bf->mbits = bf->nelems * (sizeof(uint32_t) << 3);
        bf->nh = (bf->mbits / bf->nelems) * log(2); 

        bf->buf = ALLOC_FILTER_BUF(bf->nelems);
        if(bf->buf == NULL) {
                free(bf);
                perror("Alloc failed, exiting...");
                exit(ALLOC_ERR);
        }

        return bf;
}

struct bloom_filter *bloom_init_nhashes(uint32_t cap, uint32_t nhashes) 
{
        struct bloom_filter *bf = ALLOC_FILTER;
        if(bf == NULL) {
                perror("Alloc failed, exiting...");
                exit(ALLOC_ERR);
        }

        bf->nelems = cap;
        bf->mbits = bf->nelems * (sizeof(uint32_t) << 3);
        bf->nh = nhashes; 

        bf->buf = ALLOC_FILTER_BUF(bf->nelems);
        if(bf->buf == NULL) {
                free(bf);
                perror("Alloc failed, exiting...");
                exit(ALLOC_ERR);
        }

        return bf;
}

void bloom_insert_int(struct bloom_filter *bf, const int32_t data)
{
        const int32_t *p = &data;
        uint32_t hash[4];
        
        MurmurHash3_x86_32((void *)p, sizeof(int32_t), 0, hash);
        for(uint32_t i = 0; i < bf->nh; ++i) 
                SET_BIT(bf, G(hash, i, p, bf->mbits));
}

void bloom_insert_string(struct bloom_filter *bf, const char *data)
{
        uint32_t hash[4];
        
        MurmurHash3_x86_32((void *)data, strlen(data), 0, hash);
        for(uint32_t i = 0; i < bf->nh; ++i)
                SET_BIT(bf, G(hash, i, data, bf->mbits));
}

bool bloom_query_int(struct bloom_filter *bf, const int32_t data)
{
        const int32_t *p = &data;
        uint32_t hash[4];

        MurmurHash3_x86_32((void *)p, sizeof(int32_t), 0, hash);
        for(uint32_t i = 0; i < bf->nh; ++i) 
                if(!TEST_BIT(bf, G(hash, i, p, bf->mbits)))
                        return false;
        return true;
}

bool bloom_query_string(struct bloom_filter *bf, const char *data)
{
        uint32_t hash[4];

        MurmurHash3_x86_32((void *)data, strlen(data), 0, hash);
        for(uint32_t i = 0; i < bf->nh; ++i) 
                if(!TEST_BIT(bf, G(hash, i, data, bf->mbits)))
                        return false;
        return true;
}