#ifndef ARTICLE_H // 防止重复包含
#define ARTICLE_H

// 定义链表节点
typedef struct Article {
    char* num;                 // 文章标号
    char* title;               // 存储文章标题
    char* content;             // 存储文章内容
    char* jpg;                 // 文章配图
    struct Article *next;      // 指向下一个节点的指针
} Article;

#endif // ARTICLE_H


int epub(char* home_location, char* txtname ,FILE* log);
int content_opf_create( struct Article* head, char* home_location, FILE* log );
int toc_ncx_create( struct Article* head, char* home_location, FILE* log  );
int chapter_create( struct Article* head, char* home_location, FILE* log );
int index_create( struct Article* head, char* home_location, FILE* log );

char* read_txt( char* txtname);  //将txt读入内存中，返回字符数组指针

struct Article * buil_article_list(char* txt_content );  // 根据txt建立链表结构
void free_article_list( struct Article* head ); // 释放链表资源