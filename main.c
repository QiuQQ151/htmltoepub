#include "./lib/head.h"

int
main(int argc,char* argv[]){

    FILE* log = fopen("log.txt","w");
    // 用于定义抓取的时间
    char* time = (char*)malloc( sizeof(char)*9 );
    *time = '\0';
    *(time+8) = '\0';

    if( argc == 1 ){
        // 默认最新日期
        // 获取当前时间
        char* local_time = get_time();
        html_fix(time,local_time,4);
        html_fix(time+4,local_time+5,2);
        html_fix(time+6,local_time+8,2);
        printf("抓取时间：%s\n",time);
        free(local_time);
    }
    else if( argc == 2 ){
        // 指定日期 例子：20241024
        if( strlen( argv[1] ) !=8 ){
            // 参数错误
            printf("输入日期错误，参考：20241024\n");
            return 1;
        }
        time = argv[1];
        printf("抓取时间：%s\n",time);
    }
    //extract_html_printf("https://epaper.nfnews.com/nfdaily/html/202410/25/content_10115891.html",log);
    extract_nfrb( time, log);
    extract_xhrb( time, log);

    free(time);
    fclose(log);
    return 0;
}