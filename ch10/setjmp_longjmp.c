#include <setjmp.h>
#include <stdio.h>

/* 示範 setjmp 和 longjmp 的使用方式，請特別注意，只能 longjmp 到 caller */
/* 可以試試看 longjmp 到 callee 看看，如果 longjmp 到 callee 會造成錯誤 */

jmp_buf buf;

int b() {
    puts("stat of b");
    longjmp(buf, 5); // 回傳值可以是任意數字，例如：5，但請不要回傳 0 以免造成混淆
    puts("end of b");
}

int a() {
    puts("stat of a");
    b();
    puts("end of a");
}

int main(int argc, char** argv) {
    int ret;
    register int p1=11;
    volatile int p2=22;
    int p3=33;
    p1=1;
    p2=2;
    p3=3;

    if ((ret=setjmp(buf)) == 0) {  // 回傳 0 代表 setjmp 成功
        a();
    } else {
        printf("return form longjmp."" the return value is %d\n", ret);
        printf("p1 = %d, p2 = %d, p3 = %d\n", p1, p2, p3);
    }
}