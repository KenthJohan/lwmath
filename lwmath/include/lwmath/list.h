#pragma once


typedef struct list_head_t
{
	struct list_head_t *next;
	struct list_head_t *prev;
} list_head_t;
