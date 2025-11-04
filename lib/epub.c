#include "./head.h"

/*
file_location:根目录 例子：./temp/xhrb/
*/
int epub(char* home_location, char* txtname, FILE* log)
{   
   // 读取txt内容到内存
   char* content;
   content = read_txt(txtname);

   // 将内容转化为链表
   struct Article* head;
   head = buil_article_list(content);
   //printf("标题:%s\n",head->title);
   
   // 创建content.opf
   content_opf_create(head,home_location,log);
   // 创建toc.ncx
   toc_ncx_create(head,home_location,log);
   // 创建chapterx.html
   chapter_create(head,home_location,log);
   // 创建index.html
   index_create(head,home_location,log);

   // 释放资源
   free_article_list(head);
   free(content); 
   return 0;

}


/*
创建content.opf
*/
int content_opf_create( struct Article* head, char* home_location, FILE* log )
{    
    char* location = (char*)malloc( sizeof(char)*( strlen(home_location) + strlen("epub/OEBPS/content.opf") + 1 ) );
    *location = '\0';
    strcat( location,home_location);
    strcat(location,"epub/OEBPS/content.opf");

    FILE* content_opf_fp;
    content_opf_fp = fopen(location,"w"); //写入模式打开
    if( content_opf_fp == NULL ){
        printf("无法打开content,opf\n");
        return 1;
    }

    // 写入metadata
    fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n",content_opf_fp);
    fputs("<package xmlns=\"http://www.idpf.org/2007/opf\" version=\"2.0\" unique-identifier=\"BookID\"> \n",content_opf_fp);
    fputs("<metadata xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n",content_opf_fp);
    fputs("<dc:title>每日新闻集</dc:title>\n",content_opf_fp);
    fputs("<dc:creator>QQQ制作</dc:creator>\n",content_opf_fp);
    fputs("<dc:language>zh</dc:language>\n",content_opf_fp);
    fputs("<dc:identifier id=\"BookID\">urn:uuid:12345</dc:identifier>\n",content_opf_fp);
    fputs("<meta name=\"cover\" content=\"cover-image\"/>\n</metadata>\n",content_opf_fp);

    struct Article* now = head;
    struct Article* next;
    // 写入manifest
    fputs("<manifest>\n",content_opf_fp);
    fputs("<item id=\"index\" href=\"index.html\" media-type=\"application/xhtml+xml\"/>\n",content_opf_fp);
    fputs("<item id=\"ncx\" href=\"toc.ncx\" media-type=\"application/x-dtbncx+xml\"/>\n",content_opf_fp);
    //    正文
    while( now->next != NULL )
    {
       fputs("<item id=\"chapter",content_opf_fp);
       fputs(now->num,content_opf_fp);
       fputs("\" href=\"chapter",content_opf_fp);
       fputs(now->num,content_opf_fp);
       fputs(".html\" media-type=\"application/xhtml+xml\"/>\n",content_opf_fp);
       next = now->next;
       now = next;
    }
    //     图片 <item id="image1" href="images/image1.jpg" media-type="image/jpeg"/>
    now = head;
    while( now->next != NULL )
    {
       if( now->jpg[0] != 'n' ){
         //有配图
            fputs("<item id=\"image",content_opf_fp);
            fputs(now->num,content_opf_fp);
            fputs("\" href=\"images/",content_opf_fp);
            fputs(now->jpg,content_opf_fp);
            fputs("\" media-type=\"aimage/jpeg\"/>\n",content_opf_fp);         
       }
       next = now->next;
       now = next;
    }
    fputs("</manifest>\n",content_opf_fp);
    
    // 写入spine
    fputs("<spine toc=\"ncx\">\n",content_opf_fp);
    fputs("<itemref idref=\"index\"/>",content_opf_fp);
    now = head;
    while( now->next != NULL )
    {
       fputs("<itemref idref=\"chapter",content_opf_fp);
       fputs(now->num,content_opf_fp);
       fputs("\"/>\n",content_opf_fp);
       next = now->next;
       now = next;
    }
    fputs("</spine>\n",content_opf_fp);
    fputs("<guide>\n<reference type=\"cover\" title=\"Cover\" href=\"cover.jpg\"/>\n</guide>\n",content_opf_fp);
    fputs("</package>\n",content_opf_fp);

    fclose(content_opf_fp);
    return 0;
}

/*
创建toc.ncx
*/
int toc_ncx_create( struct Article* head, char* home_location, FILE* log  )
{
    char* location = (char*)malloc( sizeof(char)*( strlen(home_location) + strlen("epub/OEBPS/toc.ncx") + 1 ) );
    *location = '\0';
    strcat( location,home_location);
    strcat(location,"epub/OEBPS/toc.ncx");
    
    FILE* toc_ncx_fp;
    toc_ncx_fp = fopen(location,"w"); //写入模式打开
    if( toc_ncx_fp == NULL ){
        printf("无法打开toc.ncx\n");
        return 1;
    }
    // 写入基本前缀
    fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n",toc_ncx_fp);
    fputs("<ncx xmlns=\"http://www.daisy.org/z3986/2005/ncx/\" version=\"2005-1\">\n",toc_ncx_fp);

    fputs("<head>\n",toc_ncx_fp);
    fputs("<meta name=\"dtb:uid\" content=\"urn:uuid:12345\"/>\n",toc_ncx_fp);
    fputs("<meta name=\"dtb:depth\" content=\"1\"/>\n",toc_ncx_fp);
    fputs("<meta name=\"dtb:totalPageCount\" content=\"0\"/>\n",toc_ncx_fp);
    fputs("<meta name=\"dtb:maxPageNumber\" content=\"0\"/>\n",toc_ncx_fp);
    fputs("</head>\n",toc_ncx_fp);

    fputs("<docTitle>\n",toc_ncx_fp);
    fputs("<text>新闻目录</text>\n",toc_ncx_fp);
    fputs("</docTitle>\n",toc_ncx_fp);
    
    // 写入navMap
    fputs("<navPoint id=\"index\" playOrder=\"1\">\n",toc_ncx_fp);
    fputs("<navLabel><text>新闻目录</text></navLabel>\n",toc_ncx_fp);
    fputs("<content src=\"index.html\"/>\n",toc_ncx_fp);
    struct Article* now = head;
    struct Article* next;
    fputs("<navMap>\n",toc_ncx_fp);
    while( now->next != NULL )
    {   
        char* num_Order = num2char( atoi(now->num) + 1 ); 
        fputs("<navPoint id=\"navPoint-",toc_ncx_fp);
        fputs(now->num,toc_ncx_fp);
        fputs("\" playOrder=\"",toc_ncx_fp);
        fputs(num_Order,toc_ncx_fp);
        fputs("\">\n",toc_ncx_fp);

        fputs("<navLabel><text>",toc_ncx_fp);
        fputs(now->title,toc_ncx_fp);
        fputs("</text></navLabel>\n",toc_ncx_fp);

        fputs("<content src=\"chapter",toc_ncx_fp);
        fputs(now->num,toc_ncx_fp);
        fputs(".html\"/>\n",toc_ncx_fp);

        fputs("</navPoint>\n",toc_ncx_fp);

        next = now->next;
        now = next;
    }
    fputs("</navMap>\n",toc_ncx_fp);
    fputs("</ncx>\n",toc_ncx_fp);
    fclose(toc_ncx_fp);
}

/*
创建chapter.html
*/
int chapter_create( struct Article* head, char* home_location, FILE* log )
{
    struct Article* now = head;
    struct Article* next;
    
    // 循环创建chapterx.html
    while( now->next != NULL )
    {
        char* location = (char*)malloc( sizeof(char)*( strlen(home_location) + strlen("epub/OEBPS/chapter") + 1 ) );
        *location = '\0';
        strcat( location,home_location);
        strcat(location,"epub/OEBPS/chapter");

        FILE* chapter_html_fp;
        char* chapterx = (char*)malloc( sizeof(char)*( strlen(location) + strlen(now->num) + 5 + 1));
        if( chapterx == NULL ){
            printf("无法分配chapterx内存\n");
            return 1;
        }  
        *chapterx = '\0';
        strcat(chapterx,location);
        strcat(chapterx,now->num);
        strcat(chapterx,".html");
        chapter_html_fp = fopen(chapterx,"w"); //写入模式打开
        if( chapter_html_fp == NULL ){
            printf("无法打开chapterx.html\n");
            return 1;
        }       
        
        // 写入数据
        fputs("<!DOCTYPE html>\n",chapter_html_fp);
        fputs("<html xmlns=\"http://www.w3.org/1999/xhtml\">\n",chapter_html_fp);
        fputs("<head>\n",chapter_html_fp);
        fputs("<title>",chapter_html_fp);
        fputs(now->title,chapter_html_fp);
        fputs("</title>\n",chapter_html_fp);
        fputs("</head>\n",chapter_html_fp);

        fputs("<body>\n",chapter_html_fp);
        // 标题
        fputs("<h1>",chapter_html_fp);
        fputs(now->title,chapter_html_fp);
        fputs("</h1>\n",chapter_html_fp);
        // 图片 <p><img src="images/example.jpg" alt="示例图片" /></p>
        if( now->jpg[0] != 'n'){
            // 有图片
            fputs("<img src=\"images/",chapter_html_fp);
            fputs(now->jpg,chapter_html_fp);
            fputs("\" alt=\"图\" />\n",chapter_html_fp);            
        }
        // 正文
        fputs("<p>\u3000\u3000",chapter_html_fp);
        // 增加换行符
        char* content_left = now->content;
        char* content_right = strstr(content_left,"\n");
        while( content_right !=NULL )
        { 
          *content_right = '\0';
          fputs(content_left,chapter_html_fp);
          // 更新段的起始处
          content_left = content_right + 1;
          // 检测新段回车结束符
          content_right = strstr(content_left,"\n");
          if( content_right == NULL ){
            // 没检测到结束符，本文结束
            break;
          }
          // 新的段有效，先插入换行符
          fputs("<br>\u3000\u3000",chapter_html_fp);
        }
        fputs("</p>\n",chapter_html_fp);
        fputs("</body>\n",chapter_html_fp);
        fputs("</html>\n",chapter_html_fp);

        // 释放资源
        fclose(chapter_html_fp);

        next = now->next;
        now = next; 
    } 
     return 0;

}

/*
创建index
*/
int index_create( struct Article* head, char* home_location, FILE* log )
{
    FILE* index_fp;
    char* location = (char*)malloc( sizeof(char)*( strlen(home_location) + strlen("epub/OEBPS/index.html") + 1 ) );
    *location = '\0';
    strcat( location,home_location);
    strcat(location,"epub/OEBPS/index.html");

    index_fp = fopen(location,"w"); //写入模式打开
    if( index_fp == NULL ){
        printf("无法打开index.html\n");
        return 1;
    }
    // 写入基本前缀
    fputs("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n",index_fp);
    fputs("<html xmlns=\"http://www.w3.org/1999/xhtml\">\n",index_fp);
    fputs("<head>\n",index_fp);
    fputs("<title>新闻目录</title>\n",index_fp);
    fputs("</head>\n",index_fp);

    fputs("<body>\n",index_fp);
    fputs("<h1>新闻目录</h1>\n",index_fp);
    fputs("<ul>\n",index_fp);
    // <li><a href="chapter3.html">文章 3: 第三篇文章标题</a></li>
    struct Article* now = head;
    struct Article* next;
    while( now->next !=NULL )
    {
       fputs("<li><a href=\"chapter",index_fp);
       fputs( now->num,index_fp);
       fputs(".html\">",index_fp);
       fputs( now->title,index_fp);
       fputs( "</a></li>\n",index_fp);
       next = now->next;
       now = next;
    }
    fputs("</ul>\n",index_fp);
    fputs("</body>\n",index_fp);
    fputs("</html>\n",index_fp);
    fclose(index_fp);
}

/*
// 根据txt建立链表结构
输入值：
txt内容数组
返回值：
链表头指针
*/
struct Article * buil_article_list(char* txt_content )
{   
    // 链表头
    struct Article* head = NULL;
    struct Article* pass = NULL;
    struct Article* now = NULL;
    struct Article* next = NULL;
    head =( struct Article* )malloc( sizeof( struct Article) );
    head->next = NULL; 
    now = head;
    next = now;

    // 内容定位
    char* jpg_left;
    char* jpg_right;
    char* tiltle_left  ;
    char* tiltle_right  ;
    char* content_left  ;
    char* content_right ;
    char* article_num;

    // jpg内容定位首次定位
    jpg_left = strstr(txt_content,"<图片:");
    if( jpg_left == NULL ){
        printf("部分内容缺失，链表创建结束\n");
        return NULL ;  // 错误返回
    }
    jpg_left += sizeof("<图片:") - 1; //跳过标签
    
    // 开始建立链表
    int i = 1;
    fprintf(stderr, "[");
    while( 1 )
    {  
       fprintf(stderr, "*");
       // 更新jpg右侧地址
       jpg_right = strstr(jpg_left,">"); 
        if( jpg_right == NULL ){
            // 撤销当前节点
            pass->next = NULL; // 切断连接
            now = pass;
            free(next); // 释放资源
            printf("部分内容缺失，链表创建结束\n");
            return NULL ;  // 错误返回
        }

       // 更新标题左侧地址
        tiltle_left = strstr(jpg_right + 2,"<标题:"); // 注意跳过'>\n' 
        if( tiltle_left == NULL ){
            // 已无新的内容
            printf("链表创建结束\n");
            return head ;  // 结束返回
        }   
        tiltle_left += sizeof("<标题:") -1;

       // 标题右侧地址更新
       tiltle_right = strstr(tiltle_left,">"); 
        if( tiltle_right == NULL ){
            // 撤销当前节点
            pass->next = NULL; // 切断连接
            now = pass;
            free(next); // 释放资源
            printf("部分内容缺失，链表创建结束\n");
            return NULL ;  // 错误返回
        }
       // 正文左侧地址更新
       content_left = strstr( tiltle_right, "\n" );
        if( content_left == NULL ){
            // 撤销当前节点
            pass->next = NULL; // 切断连接
            now = pass;
            free(next); // 释放资源
            printf("部分内容缺失，链表创建结束\n");
            return NULL ;  // 错误返回
        }       
       content_left += 1; // 跳过\n
       // 正文右侧地址更新
       content_right = strstr( content_left, "<end>" );
        if( content_left == NULL ){
            // 撤销当前节点
            pass->next = NULL; // 切断连接
            now = pass;
            free(next); // 释放资源
            printf("部分内容缺失，链表创建结束\n");
            return NULL ;  // 错误返回
        }      

       // 写入jpg信息  //注意强制改为'\0'会导致的提前结束问题
       now->jpg = (char*)malloc( sizeof(char)*(jpg_right - jpg_left +1) );
       if( now->jpg == NULL ){
            // 撤销当前节点
            pass->jpg = NULL; // 切断连接
            now = pass;
            free(next); // 释放资源
            printf("内存分配失败，链表创建结束\n");
            return NULL ;  // 错误返回
       }
       now->jpg[0] = '\0';   // 还有没有别的办法？
       *(jpg_right) = '\0'; //修改方便字符串复制
       strcat(now->jpg, jpg_left );
       //printf("图片:%s",now->jpg);

       // 写入文章标号
       article_num = num2char( i++ );
       now->num = (char*)malloc( sizeof(char)*( sizeof(article_num ) + 1 ) );
       if( now->num == NULL ){
            // 撤销当前节点
            pass->next = NULL; // 切断连接
            now = pass;
            free(next); // 释放资源
            printf("内存分配失败，链表创建结束\n");
            return NULL ;  // 错误返回
       }
       now->num[0] = '\0';
       strcat(now->num, article_num);
       //printf("文章标号:%s\n",now->num);

       // 写入标题信息  //注意强制改为'\0'会导致的提前结束问题
       now->title = (char*)malloc( sizeof(char)*(tiltle_right - tiltle_left +1) );
       if( now->title == NULL ){
            // 撤销当前节点
            pass->next = NULL; // 切断连接
            now = pass;
            free(next); // 释放资源
            printf("内存分配失败，链表创建结束\n");
            return NULL ;  // 错误返回
       }
       now->title[0] = '\0';   // 还有没有别的办法？
       *(tiltle_right) = '\0'; //修改方便字符串复制
       strcat(now->title, tiltle_left );
       //printf("标题:%s\n",now->title);
       
       //写入正文信息 // 短尾均增加"\n   "4字符方便后面处理
       now->content = (char*)malloc( sizeof(char)*(content_right - content_left + 4 + 1) );
       if( now->content == NULL ){
            // 撤销当前节点
            pass->next = NULL; // 切断连接
            now = pass;
            free(next); // 释放资源
            printf("内存分配失败，链表创建结束\n");
            return NULL ;  // 错误返回
       }
       now->content[0] = '\0';
       *(content_right) = '\0';   //  *(content_right+1) = '\0';   越界？
       strcat(now->content, content_left );
       strcat(now->content, "\n   " );


       // 更新jpg左侧地址
        jpg_left = strstr(content_right + 3,"<图片:"); // 注意跳过'\0' 
        if( jpg_left == NULL ){
            // 已无新的内容
            fprintf(stderr, "]\n");
            printf("链表创建结束\n");
            return head ;  // 结束返回
        }   
        jpg_left += sizeof("<图片:") - 1; //跳过标签
    
        // 建立新节点
        next = (struct Article*)malloc( sizeof( struct Article ) );
        next->next = NULL;
        now->next = next;  // 建立连接
        pass = now;  // 记录上一个节点
        now = next;   // 切换
    }
   
}

/*
// 释放链表资源
*/
void free_article_list( struct Article* head )
{
   struct Article* now = head;
   struct Article* next = now->next;
   printf("开始释放链表\n");
   int i=0;
    fprintf(stderr, "[");
   while( 1 )
   {
        //printf("释放节点%d\n",i++);
        fprintf(stderr, "*");
        // 记录下个节点地址
        next = now->next; 
        // 释放当前节点资源
        free(now->jpg);
        free(now->num);
        free(now->title);
        free(now->content);
        free(now);
        now = next; // 切换到下一个节点操作
        if( now == NULL ){
             fprintf(stderr, "]\n");
            printf("链表释放结束\n");
            return;
        }
           
   }
}


/*
//将txt读入内存中，返回字符数组指针
*/
char* read_txt( char* txtname )
{
    FILE *file;
    char *content = NULL;  // 指向字符数组的指针
    long file_size;
    // 打开文件
    file = fopen( txtname , "r");
    if (file == NULL) {
        perror("无法打开文件");
        return NULL;
    }

    // 获取文件大小
    fseek(file, 0, SEEK_END);          // 移动到文件末尾
    file_size = ftell(file);           // 获取文件指针的位置，即文件大小
    fseek(file, 0, SEEK_SET);          // 移动回文件开头

    // 分配内存
    content = malloc(file_size + 1);  // +1 以便于存储字符串结束符
    if (content == NULL) {
        perror("内存分配失败");
        fclose(file);
        return NULL;
    }

    // 读取文件内容到内存
    fread(content, 1, file_size, file); // 读取文件内容
    content[file_size] = '\0';          // 添加字符串结束符    

    // 打印内容
    //printf("文件内容:\n%s\n", content);    

    // 关闭文件
    fclose(file);

    return content;
}
    
    











