#include "./head.h"

/*
新华日报抓取主函数
输入：
time：时间，例子：20241013
output_file: 输出文件操作符
*/
void extract_xhrb(char* time, FILE *log)
{  
    
    // 转换日期
    char* date = time2date(time); 

    // 合成txt名字
    char* txt_name =(char*)malloc( sizeof(char)*strlen("temp/xhrb/XHRB20241024.txt  ") );
    *txt_name = '\0';
    strcat(txt_name,"temp/xhrb/XHRB");
    strcat(txt_name,time);
    strcat(txt_name,".txt");

    // 新建txt
    FILE* output_file = fopen(txt_name,"w");
    if(output_file == NULL ){
        // 创建文件失败
        printf("XHRB创建失败%s\n",txt_name);
        return;
    }
   fputs("每日新闻\n",output_file);
   fputs("日期：",output_file);
   fputs(date,output_file);
   fputs("\n制作：QQQ\n\n\n",output_file);    

    // 提取新闻标号    // 格式https://xh.xhby.net/pc/layout/202410/25/node_1.html
    char* basic_html_first = "https://xh.xhby.net/pc/layout/";
    char* basic_html_end = "/node_1.html";
    char* article_num = get_article_firstnum( basic_html_first,basic_html_end, date, log);
    int num = atoi( article_num); // 转换为整数形式
    printf("提取到的xhrb文章标号%s\n[",article_num);
    
    // 提取新闻内容
    char* html; 
    char* jpg_start_html = "https://doss.xhby.net/zpaper/xhrb/pc/pic/"; 
    char* jpg_tag = "<img";
    char* jpg_tag_left = "../pic/";
    char* jpg_tag_right = "\">";

    char* title_tag = "newsdetatit"; 
    char* title_tag_left =  "<h3>";
    char* title_tag_right = "</h3>";

    char* content_tag = "<founder-content>";
    char* content_tag_left = "<!--enpcontent-->";
    char* content_tag_right = "<!--enpproperty";
    char* secion_left = "<p>";
    char* section_right = "</p>";

    char* jpg_location = "temp/xhrb/epub/OEBPS/images/";

    int count = 1; // 实际的文章标号
    int i = 80;
    fprintf(stderr, "[");
    while( i-- )
    {   
        // 文章地址合成 例子：https://xh.xhby.net/pc/con/202410/25/content_1382411.html
        html = makeup_article_html(date, num2char(num), "https://xh.xhby.net/pc/con/", "/content_", ".html", log);
        //printf("文章地址：%s\n",html);
        // 提取内容
        int ret = extract_article( html, &count, jpg_start_html ,
                                    jpg_tag,  jpg_tag_left,  jpg_tag_right,
                                    title_tag,  title_tag_left, title_tag_right,
                                    content_tag,  content_tag_left, content_tag_right,
                                    secion_left,  section_right,
                                    jpg_location,  output_file,  log
                                    );
        if( ret == 1 ){
             fprintf(stderr, "x");
        } else{
             fprintf(stderr, "*");
        } 
        num++;
    }
    printf("]\n");
    // 释放资源
    fclose(output_file);
     
    // 创建电子书封面
    printf("创建电子书封面\n");
    // overlay_text_on_image("lib/xhrbcover.jpg","temp/xhrb/epub/OEBPS/cover.jpg" , time, 300, 1200, 150);
    char year[5] ;
    year[4] = '\0';
    html_fix(year,time,4);
    overlay_text_on_image("lib/kedaya.jpg","temp/xhrb/epub/OEBPS/cover.jpg" , "新华日报", 120, 100, 200);
    overlay_text_on_image("temp/xhrb/epub/OEBPS/cover.jpg","temp/xhrb/epub/OEBPS/cover.jpg" , year, 85, 540, 100);
    overlay_text_on_image("temp/xhrb/epub/OEBPS/cover.jpg","temp/xhrb/epub/OEBPS/cover.jpg" , time+4, 85, 640, 100);
    epub("temp/xhrb/", txt_name , log);
} 

