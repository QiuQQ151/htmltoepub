#include "./head.h"
/*
此文件定义了通用的网页抓取工具
*/


/*
获取系统时间
输出：
时间字符串
*/
char* get_time(void)
{
    // 获取当前系统时间
    time_t now = time(NULL);
    if (now == -1) {
        perror("无法获取系统时间\n");
        return NULL;
    }

    // 转换为本地时间结构体
    struct tm* local_time = localtime(&now);

    // 格式化时间为 "YYYY-MM"
    // YYYY-MM 占用 7 个字符 + 1 个 '\0'
    char* time_str = (char*)malloc( sizeof(char)*40 );
    strftime(time_str, 40, "%Y-%m-%d %H:%M:%S", local_time);
    *(time_str+19) ='\0';
    return time_str;
}

/*
日志记录，自带换行
输入：
message：记录事件
log:记录保存位置
*/
void log_record(char* message,FILE*log)
{
   char* time = get_time();
   fputs(time,log);
   fputs("  ",log);
   fputs(message,log);
   fflush(log);
   free(time);
}

/*
// 20241024->202410/24
*/
char* time2date(char* time)
{
   // 1---生成对应日期格式
   char* date = (char*)malloc( sizeof(char)*10); // 转换日期格式
   if( date == NULL ){
      // 错误返回
      return NULL;
   }
   strncpy(date,time,6);
   *(date+6) = '/';
   strncpy(date+7,time+6,2);
   *(date+9) = '\0';
   return date;  
}

/*
//替换html指定位置的num个字符
输入：
dest：html
soure：字符来源
num：个数
*/
void html_fix(char* dest, char* source, int num )
{
  for(int i = 0; i<num; i++ ){
       *(dest+i) = *(source+i);
  }
}

/*
整数转化为字符串
*/
// 整数转化为字符串的函数
char* num2char(int num) {
    // 计算所需的字符串长度（包括符号和'\0'结尾符）
    int length = snprintf(NULL, 0, "%d", num) + 1;
    // 动态分配内存以存储字符串
    char* str = (char*)malloc(length);
    if (str == NULL) {
        perror("内存分配失败\n");
        return NULL;
    }
    // 将整数转换为字符串并存入str
    snprintf(str, length, "%d", num);
    return str;  // 返回指向字符串的指针
}

/*
回调函数：将HTTP响应数据写入内存
处理文本的
*/
size_t write_to_memory(void *contents, size_t size, size_t nmemb, void *userp,FILE *log) {
    size_t total_size = size * nmemb;
    struct Memory *mem = (struct Memory *)userp;

    char *temp = realloc(mem->data, mem->size + total_size + 1);
    if (temp == NULL) {
        log_record("回调函数内存申请失败\n",log);
        return 0;
    }

    mem->data = temp;
    memcpy(&(mem->data[mem->size]), contents, total_size);
    mem->size += total_size;
    mem->data[mem->size] = '\0'; // 添加字符串结束符
    return total_size;
}


/*
写入回调函数，将数据写入文件，处理图片的
*/
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

/*
获取指定网页内容
返回值：
网页内容字符串指针
异常返回NULL
*/
char* extract_html_data(char* html, FILE* log)
{
    log_record("开始抓取指定网页内容\n",log);
    // 资源请求，最后要释放！
    CURL *curl;
    CURLcode res;
    struct Memory chunk = {0}; //结构体全部赋值为0
    // 初始化libcurl
    curl = curl_easy_init();
    if (!curl) {
        log_record("无法初始化libcurl，错误返回\n",log); 
        return NULL;
    }
    log_record("curl资源请求完成\n",log);

    //配置回调函数（指定的curl，设置参数名称，设定值）
    curl_easy_setopt(curl, CURLOPT_URL, html);  //定义网址查找的网页
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_memory); //写入内存
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);  //
    log_record("curl设定完成\n",log);
    fflush(log);

    // 执行HTTP请求
    res = curl_easy_perform(curl);
    if ( res != CURLE_OK ) {
        log_record("HTTP请求失败，错误返回\n",log);
        fprintf(stderr, "请求失败: %s\n", curl_easy_strerror(res));
        // 释放内存
        curl_easy_cleanup(curl);
        free(chunk.data);
        fflush(log);
        return NULL;
    }
    log_record("http请求完成\n",log);
    char* ret = (char*)malloc( sizeof(char)*( strlen(chunk.data)+1 ) );
    if( ret == NULL ){
        // 内存分配失败
        log_record("无法为返回内容申请内存\n",log);
        // 释放内存
        curl_easy_cleanup(curl);
        free(chunk.data);
        fflush(log);
        return NULL;
    }
    *ret = '\0';
    strcat( ret,chunk.data );
    // 释放内存
    curl_easy_cleanup(curl);
    free(chunk.data);
    log_record("返回指定网页内容\n",log);
    fflush(log);
    return ret;
}

/*
打印指定网页内容，用于查看网页数据
*/
void  extract_html_printf(char* html, FILE* log)
{
  char* data = extract_html_data( html, log);
  if( data == NULL ){
    // 异常
    printf("网页内容为空，网址无效？\n");
    return ;
  }
  printf("网页内容为：\n%s\n",data);
  
  free(data);
  return ;
} 


/*
提取指定网页文章（图片，标题，正文）
html: 文章网址
count: 文章当前计数
jpg_tag: 图片大范围定位标签
jpg_tag_left: 
jpg_tag_right:
title_tag:
title_tag_left:
title_tag_right:
content_tag:
content_tag_left:
content_tag_right:
jpg_location: 图片存放位置
outputfile: 内容保存地址
log:
返回值：
提取成功0
异常1
*/
int extract_article(char* html, int* count, char* jpg_start_html ,
                    char* jpg_tag, char* jpg_tag_left, char* jpg_tag_right,
                    char* title_tag, char* title_tag_left, char* title_tag_right,
                    char* content_tag, char* content_tag_left, char* content_tag_right,
                    char* secion_left, char* section_right,
                    char* jpg_location, FILE* output_file, FILE* log
                    )
{
   // 抓取网址内容
   char* data = extract_html_data( html,log);
   if( data == NULL ){
    // 异常返回(网页无效)
    return 1;
   }
   // 提取图片
   int jpg = extract_jpg( data, count, jpg_tag, jpg_tag_left,  jpg_tag_right, jpg_start_html, jpg_location, log );
   //printf("这是图片内容：\n%s\n",jpg);
   // 提取标题
   char* title = extract_title( data, title_tag, title_tag_left, title_tag_right, log );
   //printf("这是标题内容：\n%s\n",title);
   // 提取正文
   char* content = extract_content( data, content_tag, content_tag_left, content_tag_right, secion_left, section_right, log );
   //printf("这是正文内容：\n%s\n",content);

   if( title && content ){
        // 保存图片信息
        if( jpg == 0 ){
            // 图片信息写入txt
            fputs("<图片:",output_file);
            fputs(num2char(*count),output_file);
            fputs(".jpg>\n",output_file);      
        }
        else{
            fputs("<图片:",output_file);
            fputs("n",output_file);
            fputs(">\n",output_file);             
        }
        // 保存标题信息
        fputs(num2char(*count),output_file);
        fputs("<标题:",output_file);
        fputs(title, output_file);
        fputs(">\n",output_file);
        // 保存正文信息
        fputs(content,output_file);
        fputs("<end>",output_file);
        fputs("\n\n",output_file);

        free(data);
        free(title);
        free(content);
        *count = *count+1;
        return 0;
   }
   free(data);
   free(title);
   free(content);
   return 1;
}

/*
// 提取图片
返回值：
成功0
失败1
*/
int extract_jpg( char* data, int* count,
                 char* jpg_tag, char* jpg_tag_left, char* jpg_tag_right, 
                 char* jpg_start_html , char* jpg_store_location ,FILE* log )
{
   // 提取jpg后缀
   char* jpg_end_html = extract_concrete_content( data, jpg_tag, jpg_tag_left, jpg_tag_right, log );
   if( jpg_end_html == NULL ){
    // 异常返回
    return 1;
   }
   // 合成图片网页地址 https://epaper.nfnews.com/nfdaily/res/202410/24/c2b7dbeb-dda9-47dc-993a-c8dbb4ac854d.jpg.2
   char* jpg_html = (char*)malloc( sizeof(char)*( strlen(jpg_start_html) + strlen(jpg_end_html) + 5 ) );
   if( jpg_html == NULL ){
    free(jpg_end_html);
    return 1;
   }
   *jpg_html = '\0';
   strcat(jpg_html,jpg_start_html);
   strcat(jpg_html,jpg_end_html);
   printf("图片下载地址合成：\n%s\n",jpg_html);

   // 合成图片存储路径
   char* num = num2char(*count);
   char* jpg_location = (char*)malloc( sizeof(char)*strlen( "temp/nfrb/epub/OEBPS/images/999.jpg   " ) );
   if( jpg_location == NULL ){
      log_record("  内存分配错误\n",log);
      return 0;
   }
   *jpg_location = '\0';
   strcat(jpg_location,jpg_store_location);
   strcat(jpg_location,num);
   strcat(jpg_location,".jpg");
   // 保存在jpg进磁盘
   download_image(jpg_html,jpg_location);
    // 释放资源
    free(jpg_end_html);
    free(jpg_html); 
    free(jpg_location);
    return 0;
}

/*
// 提取标题
返回值：标题字符串
*/
char* extract_title( char* data, char* title_tag, char* title_tag_left, char* title_tag_right, FILE* log )
{
    return extract_concrete_content( data, title_tag, title_tag_left, title_tag_right, log );
}

/*
// 提取正文
返回值：正文字符串
*/
char* extract_content( char* data, 
                       char* content_tag, char* content_tag_left, char* content_tag_right, 
                       char* section_left, char* section_right,
                       FILE* log )
{
    char* content = extract_concrete_content( data, content_tag, content_tag_left, content_tag_right, log );
    if( content == NULL  ){
        // 正文异常
        free( content );
        return NULL;
    }
    else if( strlen(content) < 20 ){
        free( content);
        return NULL;
    }
    // 申请返回值内存
    char* ret = (char*)malloc( sizeof(char)*strlen(content) );
    if( ret == NULL ){
        // 异常(内存分配失败)
        free( content );
        return NULL;
    }
    *ret = '\0';
    // 正文内的标签处理
    // 去除中间的tag 默认：secion_left+正文段落+secion_right ......
    char *mid1 = content;
    char *mid2 = content;
    while(1)
    {
        // 查找一段
        mid1 = strstr(mid2, section_left);
        if ( !mid1) {
            // 结束或异常
            free(content);
            return ret;
        }          
        mid2 = strstr(mid1, section_right);
        if (!mid2 ) {
            // 结束或异常
            free(content);
            return ret;
        }
        mid1 += strlen(section_left); // 跳过标签
        // 写入一段数据
        *mid2 = '\0'; // 方便读写         
        strcat( ret , mid1 );
        *mid2 = ' '; // 取消结束符
        // 插入换行符
        strcat( ret , "\n" );
    }
}

/*
提取版面data中的具体标签中的内容
输入值：
data: 数据字符串
start_tag：起始搜索位置（大范围）
concrete_start_tag：具体位置（精确范围）
concrete_end_tag：具体位置（精确范围）
返回值：标签范围内的具体内容
异常返回：NULL
*/
char* extract_concrete_content( char* data,  char* start_tag, char* concrete_start_tag, char* concrete_end_tag, FILE *log) 
{
    // 第一步：大范围定位start
    char *start = strstr(data, start_tag); 
    if( start == NULL ){
        log_record("大范围定位失败\n",log);
        printf("大范围定位失败\n");
        return NULL;
    }
    // 第二步：小范围定位
    char *concrete_start = strstr(start, concrete_start_tag);
    if( concrete_start == NULL ){
        log_record("小范围左定位失败\n",log);
        printf("小范围左定位失败\n");
        return NULL;
    }
    concrete_start += strlen(concrete_start_tag); //跳过标签     
    char* concrete_end = strstr(concrete_start, concrete_end_tag); 
    if( concrete_end == NULL ){
        log_record("小范围右定位失败\n",log);
        printf("小范围右定位失败\n");
        return NULL;
    }
    log_record("内容定位完成\n",log);

    //读取具体的char
    char* ret = (char*)malloc( sizeof(char)*( concrete_end - concrete_start + 2 ) );
    if( ret == NULL ){
        // 异常返回
        return NULL;
    }
    *ret = '\0';
    *concrete_end = '\0';  //标记结束符
    strcat( ret, concrete_start);  //写入字符
    *concrete_end = ' ';  //取消结束符
    log_record("内容提取完成\n",log);
    //printf("内容为：\n%s\n",ret);
    // 返回
    return ret;
}


/*
// 用于提取指定日期的文章标号
start_html:网址起始
end_html:网址结束
date:网址中间插入部分
*/
char* get_article_firstnum( char* start_html, char* end_html, char* date, FILE* log )
{
   // 资源请求
   char* basic_html = (char*)malloc( sizeof(char)*( strlen(start_html) + strlen(date) + strlen(end_html) + 1 ) );
   *basic_html = '\0';
   strcat(basic_html,start_html); //前缀
   strcat(basic_html,date); //中间
   strcat(basic_html,end_html); //后缀 

   char* data = extract_html_data(basic_html,log); 

   //抽取当天的文章起始号
   char* basic_start = "href=";
   char* basic_concrete1 =  "content_";
   char* basic_concrete2 =  ".html";
   
   char* ret = extract_concrete_content( data , basic_start,basic_concrete1,basic_concrete2,log);

   // 资源释放
   free(basic_html);
   free(data);
   return ret;
}

/*
// 合成指定num文章的网页地址
*/
char* makeup_article_html(char* date,char* num, char* start_html,char* mid_html, char* end_html, FILE* log)
{
    // 文章地址合成 例子：https://epaper.nfnews.com/nfdaily/html/202410/24/content_10115758.html
    char*article_html =(char*)malloc( sizeof(char)*( strlen( date ) + strlen(num) + strlen(start_html) + strlen(mid_html) + strlen(end_html) +5) );
    *article_html = '\0';
    strcat(article_html,start_html);
    strcat(article_html,date);
    strcat(article_html,mid_html);
    strcat(article_html,num);
    strcat(article_html,end_html);
    //printf("文章地址更新为：%s\n",article_html);
    return article_html;
}


/*
下载指定网址的图片到指定保存位置
*/
void download_image(char *url, char *filename) 
{
    CURL *curl;
    FILE *fp;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(filename, "wb");
        if (fp == NULL) {
            fprintf(stderr, "无法打开文件 %s\n", filename);
            return;
        }

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
        
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            fprintf(stderr, "下载图片失败: %s\n", curl_easy_strerror(res));
        }

        fclose(fp);
        curl_easy_cleanup(curl);
    }
}







