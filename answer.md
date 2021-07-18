# JSERV-2021-linux-quiz

#### Problm alpha:

LLL:v>>(bits-c)
RRR:v<<(bits-c)

------

#### Problem beta:

MMM:((sz+mask)  & ~mask)



**DISCUSSION**

1. 舉出 Linux 核心原始程式碼裡頭 bit rotation 的案例並說明

   Because it is power of 2, and then "and" with "negative mask" eliminates the lower bits. "sz + mask" garantees the carriage.



------

#### Problem gamma:

NNN:12

**DISCUSSION**

1. 解釋上述程式碼輸出 `-` 字元數量的原理

   ​	{2 * n * 2^n-1 } = { n * 2^n }

   ​	every iteration creates 2^n child, that is n * 2^n '-' as the result.

------

#### Problem delta:

AAA:

	queue->last->next = node;
BBB:

	new_header->value;
CCC:

```c
queue->first->next = new_header->next;
node = new_header;
mtx_lock(queue->last_mutex);
if(!queue->first->next)
	queue->last = queue->first;
mtx_unlock(queue->last_mutex);
```

DISCUSSION

1. 解釋上述程式碼運作原理並指出實作缺失

   ​	Main design error is the con_pop may have problem popping the last node. If node is to be removed and new_header is to be the new first node in queue, and deciding from whether the new_header is NULL to return NULL makes it impossible to pop the last node.
   ​	Therefore, the first dummy node in the queue should never be deleted or popped. We should use the next one as the start of remaining nodes to do the operation.
   ​	My con_pop will be like

```c
	void *con_pop_refined(con_queue_t *queue)
	{
		mtx_lock(queue->first_mutex);

		node_t *dummy = queue->first;        /* First: dummy node */
		node_t *node = queue->first->next;  /* Node to be removed */

		if (!node) {
			mtx_unlock(queue->first_mutex);
			return NULL;
		}

		/* Queue not empty: retrieve data and rewire */
		void *return_value = node->value;
		queue->first->next = node->next;
		mtx_lock(queue->last_mutex);
		if(!queue->first->next)
			queue->last = queue->first;
		mtx_unlock(queue->last_mutex);
		mtx_unlock(queue->first_mutex);

		/* Free removed node and return */
		free(node);
		return return_value;
	}
```

------

#### Problem zeta:

XXX:

```
x+1;
```

YYY:

```c
	for (i = 0, val = mp->min_pool;; i++, val*= 2) {
        if (val > sz) {
            break;
        }
    }
	*ip=0;


```

**DISCUSSION**

1. 解釋上述程式碼運作原理

------



#### Problem eta:

III:

```
target_fd, POLLIN, 0
```



JJJ:

```
cl_fd, POLLIN, 0
```

**DISCUSSION**

1. 解釋上述程式碼運作原理
2. 以 [epoll](https://man7.org/linux/man-pages/man7/epoll.7.html) 系統呼叫改寫程式碼，並設計實驗來驗證 proxy 程式碼的效率



