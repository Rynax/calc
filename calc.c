#include <stdio.h>
#include "stack.h"

#define STACK_SIZE 32   //栈大小, 只有优先级顺序递增时才会用到栈

//各种操作优先级
#define PRI_RET 0
#define PRI_ADD 1
#define PRI_SUB 1
#define PRI_MUL 2
#define PRI_DIV 2
#define PRI_PAR 2	//括号优先级提高2

#define ISNUM(ch) ((ch) >= '0' && (ch) <= '9')

//表达式检查表
#define OK   0
#define ERR -1
/* 0     1    2    3    4    5    6    7
初始化  +,-   *    /  数字   (    )    = */
int check[8][8] = {
  {ERR, OK , ERR, ERR, OK , OK , ERR, ERR},
  {ERR, ERR, ERR, ERR, OK , OK , ERR, ERR},
  {ERR, ERR, ERR, ERR, OK , OK , ERR, ERR},
  {ERR, ERR, ERR, ERR, OK , OK , ERR, ERR},
  {ERR, OK , OK , OK , ERR, ERR, OK , OK },
  {ERR, OK , ERR, ERR, OK , OK , ERR, ERR},
  {ERR, OK , OK , OK , ERR, ERR, OK , OK },
  {ERR, ERR, ERR, ERR, ERR, ERR, ERR, ERR}};

typedef struct data_st {
    double num; //数字
    int prio;   //当前运算优先级
    char sign;  //运算符
}DATA;

STACK *stack = NULL;
char *expression = NULL;
int base_prio = 0;     //基础优先级

static void usage(int ret) {
    printf("Usage: ./calc [expression]\n");
    return;
}

static DATA* new_data(void) {
    DATA *d = (DATA *)calloc(1, sizeof(DATA));
    assert(d);
    return d;
}

static int get_data(DATA *data) {
    static int line = 0, column = 0;
    int fin = 0;

    assert(data);
    while(!fin) {
        switch(*expression) {
            case '(':
                column = 5;
                if(check[line][column]) return -1;
                base_prio += PRI_PAR;
                expression++;
                break;
            case ')':
                column = 6;
                if(check[line][column]) return -1;
                base_prio -= PRI_PAR;
                expression++;
                break;
            case ' ':
                expression++;
                continue;
            case '+':
                column = 1;
                if(check[line][column]) return -1;
                data->prio = base_prio + PRI_ADD;
                data->sign = *expression;
                expression++;
                fin = 1;
                break;
            case '-':
                column = 1;
                if(check[line][column]) return -1;
                data->prio += base_prio + PRI_SUB;
                data->sign = *expression;
                expression++;
                fin = 1;
                break;
            case '*':
                column = 2;
                if(check[line][column]) return -1;
                data->prio += base_prio + PRI_MUL;
                data->sign = *expression;
                expression++;
                fin = 1;
                break;
            case '/':
                column = 3;
                if(check[line][column]) return -1;
                data->prio += base_prio + PRI_DIV;
                data->sign = *expression;
                expression++;
                fin = 1;
                break;
            case '=':
                expression++;
            case '\0':
                column = 7;
                if(check[line][column]) return -1;
                data->prio = PRI_RET;   //此处允许表达式省略最后的右括号
                data->sign = '=';
                fin = 1;
                break;
            default:
                if(ISNUM(*expression)) {
                    column = 4;
                    if(check[line][column]) return -1;
                    data->num = strtod(expression, &expression);
                    break;
                }else {
                    return -1;
                }
        }
        line = column;
        column = 0;
    }
    
    return 0;
}

static int calculation(double *result) {
    DATA *cur = NULL, *next = NULL;
    int ret = 0;

    cur = new_data();

    ret = get_data(cur);
    if(ret) {
        printf("expression error.\n");
        goto exit;
    }

    while(cur->sign != '=') {
        next = new_data();
        ret = get_data(next);
        if(ret) {
            printf("expression error.\n");
            goto exit;
        }
        
        while(cur->prio >= next->prio) {
            switch(cur->sign) { //计算
                case '+':
                    next->num = cur->num + next->num;
                    break;
                case '-':
                    next->num = cur->num - next->num;
                    break;
                case '*':
                    next->num = cur->num * next->num;
                    break;
                case '/':
                    if(next->num > -0.00000001 && next->num < 0.00000001) {
                        ret = -1;
                        goto exit;
                    }
                    next->num = cur->num / next->num;
                    break;
                default:
                    ret = -1;
                    goto exit;
            }
            free(cur);
            if(!(cur = STACK_POP(stack))) {
                cur = next;
                next = NULL;
                break;
            }
        }

        if(!next) continue;
        if(!STACK_PUSH(stack, cur)) goto exit;
        cur = next;
        next = NULL;
    }

    *result = cur->num;

exit:
    if(cur) free(cur);
    if(next) free(next);
    return ret;
}

int main(int c, char **v) {
    int ret = 0;
    double result = 0.0;

    if(c != 2) usage(-1);
    assert(v[1]);

    expression = v[1];
    stack = NEW_STACK(STACK_SIZE);

    ret = calculation(&result);
    if(ret) {
        printf("calculation failed, ret = %d\n", ret);
        DESTROY_STACK(stack);
        return ret;
    }

    printf("%lf\n",result);

    DESTROY_STACK(stack);
    return 0;
}
