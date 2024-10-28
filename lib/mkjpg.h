// 用于加载UTF-8编码的中文字符
int get_next_utf8_char(const char **p);

// 读取JPEG图像
unsigned char *read_jpeg(const char *filename, int *width, int *height);

// 创建叠加文字的JPEG图像
void overlay_text_on_image(const char *input_filename, const char *output_filename, const char *text, int x,int y,int size);

