#include "leptjson.h"
#include<errno.h>
#include<math.h>
#include<assert.h>
#include<stdlib.h>

#define EXPECT(c,ch) do{assert(*c->json == (ch));c->json++;} while(0)
#define ISDIGIT(ch) ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch) ((ch) >= '1' && (ch) <= '9')

/* 防止参数过多，所以采用一个context */
typedef struct{
    const char* json;
    char* stack;
    size_t size,top;
}lept_context;

/* 解析空格 */
static void lept_parse_whitespace(lept_context* c){
    const char *p = c->json;
    while(*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')
        p++;

    c->json = p;
}

static void* lept_context_push(lept_context* c,size_t size){
    void* ret;
    assert(size > 0);
    if(c->top + size >= c->size){
        if(c->size == 0)
            c->size = LEPT_PARSE_STACK_INIT_SIZE;
        while(c->top + size >= c->size)
            c->size += c->size >> 1;    /* 扩容1.5倍 */
    }
}

/* 代码重合度过高，提取公共函数进行重构
static int lept_parse_true(lept_context* c,lept_value* v){
    EXPECT(c,'t');
    if(c->json[0] != 'r' || c->json[1] != 'u' || c->json[2] != 'e')
        return LEPT_PARSE_INVALID_VALUE;

    c->json += 3;
    v->type = LEPT_TRUE;
    return LEPT_PARSE_OK;
}

static int lept_parse_false(lept_context* c,lept_value* v){
    EXPECT(c,'f');
    if(c->json[0] != 'a' || c->json[1] != 'l' || c->json[2] != 's' || c->json[3] != 'e')
        return LEPT_PARSE_INVALID_VALUE;

    c->json += 4;
    v->type = LEPT_FALSE;
    return LEPT_PARSE_OK;
}

static int lept_parse_null(lept_context* c,lept_value* v){
    EXPECT(c,'n');
    if(c->json[0] != 'u' || c->json[1] != '1' || c->json[2] != 'l')
        return LEPT_PARSE_INVALID_VALUE;
    
    c->json += 3;
    v->type = LEPT_NULL;

    return LEPT_PARSE_OK;
}
*/

static int lept_parse_literal(lept_context* c,lept_value* v,const char* literal,lept_type type){
    size_t i;
    EXPECT(c,literal[0]);
    for(i = 0;literal[i+1];i++)
        if(c->json[i] != literal[i+1]) 
            return LEPT_PARSE_INVALID_VALUE;

    c->json += i;
    v->type = type;
    return LEPT_PARSE_OK;
}

static int lept_parse_number(lept_context* c,lept_value* v){
    const char* p = c->json;
    if(*p == '-') p++;
    if(*p == '0') p++;
    else{
        if(!ISDIGIT1TO9(*p)) return LEPT_PARSE_INVALID_VALUE;
        for(p++;ISDIGIT(*p);p++);
    }
    if(*p == '.'){
        p++;
        if(!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for(p++;ISDIGIT(*p);p++);
    }
    if(*p == 'e' || *p == 'E'){
        p++;
        if(*p == '+' || *p == '-') p++;
        if(!ISDIGIT(*p)) return LEPT_PARSE_INVALID_VALUE;
        for(p++;ISDIGIT(*p);p++);
    }
    errno = 0;
    /*
     * strtod 函数
     * 将string转化成double类型
     */
    v->u.n = strtod(c->json,NULL);
    if(errno == ERANGE && (v->u.n == HUGE_VAL || v->u.n == -HUGE_VAL)){
        return LEPT_PARSE_INVALID_VALUE;
    }
    c->json = p;
    v->type = LEPT_NUMBER;
    return LEPT_PARSE_OK;
}

static int lept_parse_value(lept_context* c,lept_value* v){
    switch(*c->json){
        case 't':   return lept_parse_literal(c,v,"true",LEPT_TRUE);
        case 'f':   return lept_parse_literal(c,v,"false",LEPT_FALSE);
        case 'n':   return lept_parse_literal(c,v,"null",LEPT_NULL);
        case '\0':  return lept_parse_number(c,v);
        default:    return LEPT_PARSE_INVALID_VALUE;
    }
}

/* JSON-text = ws value ws 分为三部分 */
int lept_parse(lept_value* v,const char* json){
    lept_context c;
    int ret;
    assert(v != NULL);
    c.json = json;
    v->type = LEPT_NULL;
    lept_parse_whitespace(&c);
    if((ret = lept_parse_value(&c,v)) == LEPT_PARSE_OK){
        lept_parse_whitespace(&c);
        if(*c.json != '\0')
            ret = LEPT_PARSE_ROOT_NOT_SINGULAR;
    }
    return ret; 
}

lept_type lept_get_type(const lept_value* v){
    assert(v != NULL);
    return v->type;
}

double lept_get_number(const lept_value* v){
    assert(v != NULL && v->type == LEPT_NUMBER);
    return v->u.n;
}

