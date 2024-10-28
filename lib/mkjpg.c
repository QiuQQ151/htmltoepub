#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jpeglib.h>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H
#include "./mkjpg.h"

// 用于加载UTF-8编码的中文字符
int get_next_utf8_char(const char **p) {
    int codepoint = 0;
    unsigned char c = **p;

    if ((c & 0x80) == 0) {
        codepoint = c;
        (*p)++;
    } else if ((c & 0xE0) == 0xC0) {
        codepoint = c & 0x1F;
        (*p)++;
        codepoint = (codepoint << 6) | (**p & 0x3F);
        (*p)++;
    } else if ((c & 0xF0) == 0xE0) {
        codepoint = c & 0x0F;
        (*p)++;
        codepoint = (codepoint << 6) | (**p & 0x3F);
        (*p)++;
        codepoint = (codepoint << 6) | (**p & 0x3F);
        (*p)++;
    } else {
        // 处理其他多字节字符...
        (*p)++;
    }
    
    return codepoint;
}

// 读取JPEG图像
unsigned char *read_jpeg(const char *filename, int *width, int *height) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    FILE *infile = fopen(filename, "rb");
    if (!infile) {
        fprintf(stderr, "无法打开输入文件 %s\n", filename);
        return NULL;
    }
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    *width = cinfo.output_width;
    *height = cinfo.output_height;
    unsigned char *image_buffer = (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * 3);
    if (!image_buffer) {
        fprintf(stderr, "内存分配失败\n");
        fclose(infile);
        return NULL;
    }

    while (cinfo.output_scanline < cinfo.output_height) {
        unsigned char *row_pointer[1];
        row_pointer[0] = &image_buffer[cinfo.output_scanline * cinfo.output_width * 3];
        jpeg_read_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_decompress(&cinfo);
    fclose(infile);
    return image_buffer;
}

// 创建叠加文字的JPEG图像
void overlay_text_on_image(const char *input_filename, const char *output_filename, const char *text, int x,int y,int size) {
    int width, height;
    unsigned char *image_buffer = read_jpeg(input_filename, &width, &height);
    if (!image_buffer) {
        return;
    }

    // 初始化 FreeType
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        fprintf(stderr, "无法初始化 FreeType 库\n");
        free(image_buffer);
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc", 0, &face)) {
        fprintf(stderr, "无法加载字体\n");
        free(image_buffer);
        FT_Done_FreeType(ft);
        return;
    }

    FT_Set_Pixel_Sizes(face,size, 0); // 设置字体大小

    // 在图像上绘制文字
    int x_offset = x; // 文字的x偏移
    int y_offset = y; // 文字的y偏移

    for (const char *p = text; *p != '\0';) {
        int unicode_char = get_next_utf8_char(&p); // 获取下一个 Unicode 字符

        // 使用 unicode_char 加载字符
        if (FT_Load_Char(face, unicode_char, FT_LOAD_RENDER)) {
            fprintf(stderr, "无法加载字符: %d\n", unicode_char);
            continue; // 跳过该字符
        }

        for (int j = 0; j < face->glyph->bitmap.rows; j++) {
            for (int k = 0; k < face->glyph->bitmap.width; k++) {
                if (face->glyph->bitmap.buffer[j * face->glyph->bitmap.width + k]) {
                    int idx = ((y_offset + j) * width + (x_offset + k)) * 3;
                    // 设置文字颜色（黑色）
                    if (idx >= 0 && idx < width * height * 3) {
                        image_buffer[idx] = 0;     // R
                        image_buffer[idx + 1] = 0; // G
                        image_buffer[idx + 2] = 0; // B
                    }
                }
            }
        }
        x_offset += face->glyph->advance.x >> 6; // 更新x坐标
    }

    // 创建输出JPEG图像
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE *outfile = fopen(output_filename, "wb");
    if (!outfile) {
        fprintf(stderr, "无法打开输出文件 %s\n", output_filename);
        free(image_buffer);
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
        return;
    }
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3; // RGB
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 100, TRUE); // 设置质量为 100%

    jpeg_start_compress(&cinfo, TRUE);

    // 写入JPEG图像数据
    while (cinfo.next_scanline < cinfo.image_height) {
        unsigned char *row_pointer[1];
        row_pointer[0] = &image_buffer[cinfo.next_scanline * width * 3];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    // 完成压缩
    jpeg_finish_compress(&cinfo);
    fclose(outfile);
    free(image_buffer);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    jpeg_destroy_compress(&cinfo);
}


