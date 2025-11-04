#include "./head.h"

/*
// 南方日报抓取主函数
传入：
抓取时间
日志保存位置
*/
void extract_nfrb(char* time, FILE *log)
{   

    // 转换日期
    char* date = time2date(time); 

    // 合成txt名字
    char* txt_name =(char*)malloc( sizeof(char)*strlen("temp/nfrb/NFRB20241024.txt  ") );
    *txt_name = '\0';
    strcat(txt_name,"temp/nfrb/NFRB");
    strcat(txt_name,time);
    strcat(txt_name,".txt");

    // 新建txt
    FILE* output_file = fopen(txt_name,"w");
    if(output_file == NULL ){
        // 创建文件失败
        printf("NFRB创建失败%s\n",txt_name);
        return;
    }
   fputs("每日新闻\n",output_file);
   fputs("日期：",output_file);
   fputs(date,output_file);
   fputs("\n制作：QQQ\n\n\n",output_file);    

    // 提取新闻标号    // 格式https://epaper.nfnews.com/nfdaily/html/202410/24/node_A01.html
    char* basic_html_first = "https://epaper.nfnews.com/nfdaily/html/";
    char* basic_html_end = "/node_A01.html";
    char* article_num = get_article_firstnum( basic_html_first,basic_html_end, date, log);
    int num = atoi( article_num); // 转换为整数形式
    printf("提取到的nfrb文章标号%s\n",article_num);
    
    // 提取新闻内容
    char* html; //  "https://epaper.nfnews.com/nfdaily/html/202410/24/content_10115761.html";
    char* jpg_start_html = "https://epaper.nfnews.com/nfdaily/"; 
    char* jpg_tag = "gallery";
    char* jpg_tag_left = "<img src=\"../../../";
    char* jpg_tag_right = "\"></td></tr>";

    char* title_tag = "primers"; 
    char* title_tag_left =  "<h1><p>";
    char* title_tag_right = "</p></h1>";

    char* content_tag = "<!--enpcontent--><html>";
    char* content_tag_left = "<body>";
    char* content_tag_right = "</body>";
    char* secion_left = "<p>&nbsp;&nbsp;&nbsp;&nbsp;";
    char* section_right = "</p>";

    char* jpg_location = "temp/nfrb/epub/OEBPS/images/";

    int count = 1; // 实际的文章标号
    int i = 120;
    fprintf(stderr, "[");
    while( i-- )
    {
        html = makeup_article_html(date, num2char(num), "https://epaper.nfnews.com/nfdaily/html/", "/content_", ".html", log);
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
        num--;
    }
    i = 20;
    num = atoi( article_num) + 1;
    while( i-- )
    {
        html = makeup_article_html(date, num2char(num), "https://epaper.nfnews.com/nfdaily/html/", "/content_", ".html", log);
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
    // 4---释放资源
    fclose(output_file);

    // 创建电子书封面
    printf("创建电子书封面\n");
    overlay_text_on_image("lib/cover.jpg","temp/nfrb/epub/OEBPS/cover.jpg" , "南方日报", 150, 250, 150);
    overlay_text_on_image("temp/nfrb/epub/OEBPS/cover.jpg","temp/nfrb/epub/OEBPS/cover.jpg" , time, 150, 420, 150);
    epub("temp/nfrb/", txt_name , log);
}

