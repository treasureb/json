#ifndef LEPTJSON_H__
#define LEPTJSON_H__

#include<stddef.h> /* size_t  */

/* 定义json默认的七种数据类型 */
typedef enum{
    LEPT_NULL,
    LEPT_FALSE,
    LEPT_TRUE,
    LEPT_NUMBER,
    LEPT_STRING,
    LEPT_ARRAY,
    LEPT_OBJECT
}lept_type;

/* json的树形结构 */
typedef struct{
    /*
     * 一个类型不可能同时为字符串或者数字
     * 所以采用联合体节省空间
     */
    union{
        struct {
            char* s;
            size_t len;
        }s;
        double n;
    }u;
    lept_type type;
}lept_value;

/* 返回对应的错误码 */
enum{
    LEPT_PARSE_OK = 0,
    LEPT_PARSE_EXPECT_VALUE,
    LEPT_PARSE_INVALID_VALUE,
    LEPT_PARSE_ROOT_NOT_SINGULAR,
    LEPT_PARSE_NUMBER_TOO_BIG,
    LEPT_PARSE_MISS_QUOTATION_MARK,
    LEPT_PARSE_INVALID_STRING_ESCAPE,
    LEPT_PAESE_INVALID_STRING_CHAR,
    LEPT_PARSE_INVALID_UNICODE_HEX,
    LEPT_PARSE_INVALID_UNICODE_SURROGATE
};

#define lept_init(v) do { (v)->type = LEPT_NULL; } while(0)

/* 解析json */
int lept_parse(lept_value* v,const char* json);

void lept_free(lept_value* v);

/* 访问结果，获取其类型 */
lept_type lept_get_type(const lept_value* v);

#define lept_set_null(v) lept_free(v)

int lept_get_boolean(const lept_value* v);
void lept_set_boolean(lept_value* v,double b);

/* get the number */
double lept_get_number(const lept_value* v);
void lept_set_number(lept_value* v,double n);

const char* lept_get_string(const lept_value* v);
size_t lept_get_string_length(const lept_value* v);
void lept_set_string(lept_value* v,const char* s,size_t len);

#endif /* LEPTJSON_H__*/
