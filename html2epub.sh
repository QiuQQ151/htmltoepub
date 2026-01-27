# 脚本

# 历史文件保留与文件夹重建
mkdir -p history && mv temp/nfrb/*.txt  history/
mkdir -p history && mv temp/nfrb/*.epub history/
mkdir -p history && mv temp/xhrb/*.txt  history/
mkdir -p history && mv temp/xhrb/*.epub history/
rm -r temp && mkdir temp

mkdir temp/nfrb
mkdir temp/nfrb/epub
mkdir temp/nfrb/epub/META-INF
mkdir temp/nfrb/epub/OEBPS
mkdir temp/nfrb/epub/OEBPS/images
cp lib/mimetype      temp/nfrb/epub/
cp lib/container.xml temp/nfrb/epub/META-INF/
cp lib/cover.jpg     temp/nfrb/epub/OEBPS/cover.jpg 

mkdir temp/xhrb
mkdir temp/xhrb/epub
mkdir temp/xhrb/epub/META-INF
mkdir temp/xhrb/epub/OEBPS
mkdir temp/xhrb/epub/OEBPS/images
cp lib/mimetype      temp/xhrb/epub/
cp lib/container.xml temp/xhrb/epub/META-INF/
cp lib/kedaya.jpg     temp/xhrb/epub/OEBPS/cover.jpg 

# 执行抓取与epub制作程序
DIR=$(pwd)
cd "$DIR/html2epub"
./html2epub.exe

# epub打包
# 选择封面图片
# largest_image=$(find "$DIR/html2epub/temp/xhrb/epub/OEBPS/images/" -type f \( -iname "*.jpg" -o -iname "*.png" -o -iname "*.gif" \) -exec du -b {} + | sort -n | tail -n 1 | awk '{print $2}')
# cp "$largest_image" "$DIR/html2epub/temp/xhrb/epub/OEBPS/cover.jpg"
#     # 特别的封面
#         # 定义目标日期
#         target_day=
#         target_month=
#         # 获取今天的日期和月份
#         today_day=$(date +%d)
#         today_month=$(date +%m)
#         # 检查今天是否是目标日期
#         if [ "$today_day" -eq "$target_day" ] && [ "$today_month" -eq "$target_month" ]; then
#             echo "今天是 ${target_month}月${target_day}日，执行特定命令。"
#             # 在这里执行你的特定命令
#             cp "$DIR/html2epub/lib/birthday.jpg" "$DIR/html2epub/temp/xhrb/epub/OEBPS/cover.jpg"
#         else
#             echo "今天不是 ${target_month}月${target_day}日。"
#         fi
for txt_file in $DIR/html2epub/temp/xhrb/*.txt; do
filename=$(basename "$txt_file" .txt)  # 如 新华日报20241016
epub_file="$DIR/html2epub/temp/xhrb/${filename}.epub"
cd "$DIR/html2epub/temp/xhrb/epub/"
zip -X0 "$epub_file" mimetype
zip -rX9 "$epub_file" META-INF/ OEBPS/
done

# largest_image=$(find "$DIR/html2epub/temp/nfrb/epub/OEBPS/images/" -type f \( -iname "*.jpg" -o -iname "*.png" -o -iname "*.gif" \) -exec du -b {} + | sort -n | tail -n 1 | awk '{print $2}')
# cp "$largest_image" "$DIR/html2epub/temp/nfrb/epub/OEBPS/cover.jpg"
for txt_file in $DIR/html2epub/temp/nfrb/*.txt; do
filename=$(basename "$txt_file" .txt)  # 如 新华日报20241016
epub_file="$DIR/html2epub/temp/nfrb/${filename}.epub"
cd "$DIR/html2epub/temp/nfrb/epub/"
zip -X0 "$epub_file" mimetype
zip -rX9 "$epub_file" META-INF/ OEBPS/
done



