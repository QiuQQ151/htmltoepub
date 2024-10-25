#ifndef CHUNK_H // 防止重复包含
#define CHUNK_H
/*
动态内存结构体，用于存储网页数据
*/
struct Memory {
    char *data;
    size_t size; //无符号整数类型，定义对象的索引或大小
};
#endif // 

// 回调函数
size_t write_to_memory(void *contents, size_t size, size_t nmemb, void *userp,FILE *log);
size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream);

// 功能函数
char* num2char(int num); //整数转化为字符串
void html_fix(char* dest, char* source, int num ); //替换html指定位置的num个字符
char* time2date(char* time); // 20241024->202410/24
char* extract_html_data(char* html, FILE* log);    // 提取指定网页内容，返回内容字符串
void  extract_html_printf(char* html, FILE* log);  // 打印指定网页内容，用于查看网页数据
void download_image(char *url, char *filename); // 下载图片到指定目录文件
int extract_article(char* html, int* count, char* jpg_start_html ,
                    char* jpg_tag, char* jpg_tag_left, char* jpg_tag_right,
                    char* title_tag, char* title_tag_left, char* title_tag_right,
                    char* content_tag, char* content_tag_left, char* content_tag_right,
                    char* secion_left, char* section_right,
                    char* jpg_location, FILE* output_file, FILE* log
                    );
int extract_jpg( char* data, int* count,
                 char* jpg_tag, char* jpg_tag_left, char* jpg_tag_right, 
                 char* jpg_start_html , char* jpg_store_location ,FILE* log ); // 提取图片
char* extract_title( char* data, char* title_tag, char* title_tag_left, char* title_tag_right, FILE* log ); // 提取标题
char* extract_content( char* data, 
                       char* content_tag, char* content_tag_left, char* content_tag_right, 
                       char* section_left, char* section_right,
                       FILE* log ); // 提取正文

char* extract_concrete_content( char* data,  char* start_tag, char* concrete_start_tag, char* concrete_end_tag, FILE *log); // 提取精确标签内容
char* get_article_firstnum( char* start_html, char* end_html, char* date, FILE* log );  // 用于提取指定日期的文章标号
char* makeup_article_html(char* date,char* num, char* start_html,char* mid_html, char* end_html, FILE* log); // 合成文章地址

// 日志记录函数
char* get_time(void); // 获取系统时间
void  log_record(char* message,FILE*log); //日志记录



